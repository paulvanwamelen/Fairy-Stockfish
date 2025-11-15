/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2022 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>   // For std::memset
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include "bitboard.h"
#include "evaluate.h"
#include "material.h"
#include "misc.h"
#include "pawns.h"
#include "thread.h"
#include "timeman.h"
#include "uci.h"
#include "incbin/incbin.h"


// Macro to embed the default efficiently updatable neural network (NNUE) file
// data in the engine binary (using incbin.h, by Dale Weiler).
// This macro invocation will declare the following three variables
//     const unsigned char        gEmbeddedNNUEData[];  // a pointer to the embedded data
//     const unsigned char *const gEmbeddedNNUEEnd;     // a marker to the end
//     const unsigned int         gEmbeddedNNUESize;    // the size of the embedded file
// Note that this does not work in Microsoft Visual Studio.
#if !defined(_MSC_VER) && !defined(NNUE_EMBEDDING_OFF)
  INCBIN(EmbeddedNNUE, EvalFileDefaultName);
#else
  const unsigned char        gEmbeddedNNUEData[1] = {0x0};
  [[maybe_unused]]
  const unsigned char *const gEmbeddedNNUEEnd = &gEmbeddedNNUEData[1];
  const unsigned int         gEmbeddedNNUESize = 1;
#endif


using namespace std;

namespace Stockfish {

const Variant* currentNnueVariant;

namespace Eval {

  bool useNNUE;
  string eval_file_loaded = "None";

  /// NNUE::init() tries to load a NNUE network at startup time, or when the engine
  /// receives a UCI command "setoption name EvalFile value nn-[a-z0-9]{12}.nnue"
  /// The name of the NNUE network is always retrieved from the EvalFile option.
  /// We search the given network in three locations: internally (the default
  /// network may be embedded in the binary), in the active working directory and
  /// in the engine directory. Distro packagers may define the DEFAULT_NNUE_DIRECTORY
  /// variable to have the engine search in a special directory in their distro.

  void NNUE::init() {

    useNNUE = Options["Use NNUE"];
    if (!useNNUE)
        return;

    string eval_file = string(Options["EvalFile"]);

    // Restrict NNUE usage to corresponding variant
    // Support multiple variant networks separated by semicolon(Windows)/colon(Unix)
    stringstream ss(eval_file);
    string variant = string(Options["UCI_Variant"]);
    useNNUE = false;
    while (getline(ss, eval_file, UCI::SepChar))
    {
        string basename = eval_file.substr(eval_file.find_last_of("\\/") + 1);
        string nnueAlias = variants.find(variant)->second->nnueAlias;
        if (basename.rfind(variant, 0) != string::npos || (!nnueAlias.empty() && basename.rfind(nnueAlias, 0) != string::npos))
        {
            useNNUE = true;
            break;
        }
    }
    if (!useNNUE)
        return;

    currentNnueVariant = variants.find(variant)->second;

    #if defined(DEFAULT_NNUE_DIRECTORY)
    #define stringify2(x) #x
    #define stringify(x) stringify2(x)
    vector<string> dirs = { "<internal>" , "" , CommandLine::binaryDirectory , stringify(DEFAULT_NNUE_DIRECTORY) };
    #else
    vector<string> dirs = { "<internal>" , "" , CommandLine::binaryDirectory };
    #endif

    for (string directory : dirs)
        if (eval_file_loaded != eval_file)
        {
            if (directory != "<internal>")
            {
                ifstream stream(directory + eval_file, ios::binary);
                if (load_eval(eval_file, stream))
                    eval_file_loaded = eval_file;
            }

            if (directory == "<internal>" && eval_file == EvalFileDefaultName)
            {
                // C++ way to prepare a buffer for a memory stream
                class MemoryBuffer : public basic_streambuf<char> {
                    public: MemoryBuffer(char* p, size_t n) { setg(p, p, p + n); setp(p, p + n); }
                };

                MemoryBuffer buffer(const_cast<char*>(reinterpret_cast<const char*>(gEmbeddedNNUEData)),
                                    size_t(gEmbeddedNNUESize));

                istream stream(&buffer);
                if (load_eval(eval_file, stream))
                    eval_file_loaded = eval_file;
            }
        }
  }

  /// NNUE::verify() verifies that the last net used was loaded successfully
  void NNUE::verify() {

    string eval_file = string(Options["EvalFile"]);

    if (useNNUE && eval_file.find(eval_file_loaded) == string::npos)
    {
        UCI::OptionsMap defaults;
        UCI::init(defaults);

        string msg1 = "If the UCI option \"Use NNUE\" is set to true, network evaluation parameters compatible with the engine must be available.";
        string msg2 = "The option is set to true, but the network file " + eval_file + " was not loaded successfully.";
        string msg3 = "The UCI option EvalFile might need to specify the full path, including the directory name, to the network file.";
        string msg4 = "The default net can be downloaded from: https://tests.stockfishchess.org/api/nn/" + string(defaults["EvalFile"]);
        string msg5 = "The engine will be terminated now.";

        sync_cout << "info string ERROR: " << msg1 << sync_endl;
        sync_cout << "info string ERROR: " << msg2 << sync_endl;
        sync_cout << "info string ERROR: " << msg3 << sync_endl;
        sync_cout << "info string ERROR: " << msg4 << sync_endl;
        sync_cout << "info string ERROR: " << msg5 << sync_endl;

        exit(EXIT_FAILURE);
    }

    if (CurrentProtocol != XBOARD)
    {
        if (useNNUE)
            sync_cout << "info string NNUE evaluation using " << eval_file_loaded << " enabled" << sync_endl;
        else
            sync_cout << "info string classical evaluation enabled" << sync_endl;
    }
  }
}

namespace Trace {

  enum Tracing { NO_TRACE, TRACE };

  enum Term { // The first PIECE_TYPE_NB entries are reserved for PieceType
    MATERIAL = PIECE_TYPE_NB, IMBALANCE, MOBILITY, THREAT, PASSED, SPACE, VARIANT, WINNABLE, TOTAL, TERM_NB
  };

  Score scores[TERM_NB][COLOR_NB];

  double to_cp(Value v) { return double(v) / PawnValueEg; }

  void add(int idx, Color c, Score s) {
    scores[idx][c] = s;
  }

  void add(int idx, Score w, Score b = SCORE_ZERO) {
    scores[idx][WHITE] = w;
    scores[idx][BLACK] = b;
  }

  std::ostream& operator<<(std::ostream& os, Score s) {
    os << std::setw(5) << to_cp(mg_value(s)) << " "
       << std::setw(5) << to_cp(eg_value(s));
    return os;
  }

  std::ostream& operator<<(std::ostream& os, Term t) {

    if (t == MATERIAL || t == IMBALANCE || t == WINNABLE || t == TOTAL)
        os << " ----  ----"    << " | " << " ----  ----";
    else
        os << scores[t][WHITE] << " | " << scores[t][BLACK];

    os << " | " << scores[t][WHITE] - scores[t][BLACK] << " |\n";
    return os;
  }
}

using namespace Trace;

namespace {

  // Threshold for lazy and space evaluation
  constexpr Value LazyThreshold1    =  Value(1565);
  constexpr Value LazyThreshold2    =  Value(1102);
  constexpr Value SpaceThreshold    =  Value(11551);

  // KingAttackWeights[PieceType] contains king attack weights by piece type
  constexpr int KingAttackWeights[PIECE_TYPE_NB] = { 0, 0, 81, 52, 44, 10, 40 };

  // SafeCheck[PieceType][single/multiple] contains safe check bonus by piece type,
  // higher if multiple safe checks are possible for that piece type.
  constexpr int SafeCheck[][2] = {
      {}, {600, 600}, {803, 1292}, {639, 974}, {1087, 1878}, {759, 1132}, {600, 900}
  };

#define S(mg, eg) make_score(mg, eg)

  // MobilityBonus[PieceType-2][attacked] contains bonuses for middle and end game,
  // indexed by piece type and number of attacked squares in the mobility area.
  constexpr Score MobilityBonus[][4 * RANK_NB] = {
    { S(-62,-79), S(-53,-57), S(-12,-31), S( -3,-17), S(  3,  7), S( 12, 13), // Knight
      S( 21, 16), S( 28, 21), S( 37, 26) },
    { S(-47,-59), S(-20,-25), S( 14, -8), S( 29, 12), S( 39, 21), S( 53, 40), // Bishop
      S( 53, 56), S( 60, 58), S( 62, 65), S( 69, 72), S( 78, 78), S( 83, 87),
      S( 91, 88), S( 96, 98) },
    { S(-60,-82), S(-24,-15), S(  0, 17) ,S(  3, 43), S(  4, 72), S( 14,100), // Rook
      S( 20,102), S( 30,122), S( 41,133), S(41 ,139), S( 41,153), S( 45,160),
      S( 57,165), S( 58,170), S( 67,175) },
    { S(-29,-49), S(-16,-29), S( -8, -8), S( -8, 17), S( 18, 39), S( 25, 54), // Queen
      S( 23, 59), S( 37, 73), S( 41, 76), S( 54, 95), S( 65, 95) ,S( 68,101),
      S( 69,124), S( 70,128), S( 70,132), S( 70,133) ,S( 71,136), S( 72,140),
      S( 74,147), S( 76,149), S( 90,153), S(104,169), S(105,171), S(106,171),
      S(112,178), S(114,185), S(114,187), S(119,221) }
  };
  constexpr Score MaxMobility  = S(150, 200);
  constexpr Score DropMobility = S(10, 10);

  // BishopPawns[distance from edge] contains a file-dependent penalty for pawns on
  // squares of the same color as our bishop.
  constexpr Score BishopPawns[int(FILE_NB) / 2] = {
    S(3, 8), S(3, 9), S(2, 8), S(3, 8)
  };

  // KingProtector[knight/bishop] contains penalty for each distance unit to own king
  constexpr Score KingProtector[] = { S(8, 9), S(6, 9) };

  // Outpost[knight/bishop] contains bonuses for each knight or bishop occupying a
  // pawn protected square on rank 4 to 6 which is also safe from a pawn attack.
  constexpr Score Outpost[] = { S(57, 38), S(31, 24) };

  // PassedRank[Rank] contains a bonus according to the rank of a passed pawn
  constexpr Score PassedRank[RANK_NB] = {
    S(0, 0), S(7, 27), S(16, 32), S(17, 40), S(64, 71), S(170, 174), S(278, 262)
  };

  constexpr Score RookOnClosedFile = S(10, 5);
  constexpr Score RookOnOpenFile[] = { S(19, 6), S(47, 26) };

  // ThreatByMinor/ByRook[attacked PieceType] contains bonuses according to
  // which piece type attacks which one. Attacks on lesser pieces which are
  // pawn-defended are not considered.
  constexpr Score ThreatByMinor[PIECE_TYPE_NB] = {
    S(0, 0), S(5, 32), S(55, 41), S(77, 56), S(89, 119), S(79, 162)
  };

  constexpr Score ThreatByRook[PIECE_TYPE_NB] = {
    S(0, 0), S(3, 44), S(37, 68), S(42, 60), S(0, 39), S(58, 43)
  };

  constexpr Value CorneredBishop = Value(50);

  // Assorted bonuses and penalties
  constexpr Score UncontestedOutpost  = S(  1, 10);
  constexpr Score BishopOnKingRing    = S( 24,  0);
  constexpr Score BishopXRayPawns     = S(  4,  5);
  constexpr Score FlankAttacks        = S(  8,  0);
  constexpr Score Hanging             = S( 69, 36);
  constexpr Score KnightOnQueen       = S( 16, 11);
  constexpr Score LongDiagonalBishop  = S( 45,  0);
  constexpr Score MinorBehindPawn     = S( 18,  3);
  constexpr Score PassedFile          = S( 11,  8);
  constexpr Score PawnlessFlank       = S( 17, 95);
  constexpr Score ReachableOutpost    = S( 31, 22);
  constexpr Score RestrictedPiece     = S(  7,  7);
  constexpr Score RookOnKingRing      = S( 16,  0);
  constexpr Score SliderOnQueen       = S( 60, 18);
  constexpr Score ThreatByKing        = S( 24, 89);
  constexpr Score ThreatByPawnPush    = S( 48, 39);
  constexpr Score ThreatBySafePawn    = S(173, 94);
  constexpr Score TrappedRook         = S( 55, 13);
  constexpr Score WeakQueenProtection = S( 14,  0);
  constexpr Score WeakQueen           = S( 56, 15);


  // Variant and fairy piece bonuses
  constexpr Score KingProximity        = S(2, 6);
  constexpr Score EndgameKingProximity = S(0, 10);
  constexpr Score ConnectedSoldier     = S(20, 20);

  constexpr int VirtualCheck = 600;

#undef S

  // Evaluation class computes and stores attacks tables and other working data
  template<Tracing T>
  class Evaluation {

  public:
    Evaluation() = delete;
    explicit Evaluation(const Position& p) : pos(p) {}
    Evaluation& operator=(const Evaluation&) = delete;
    Value value();

  private:
    template<Color Us> void initialize();
    template<Color Us> Score pieces(PieceType Pt);
    template<Color Us> Score hand(PieceType pt);
    template<Color Us> Score king() const;
    template<Color Us> Score threats() const;
    template<Color Us> Score passed() const;
    template<Color Us> Score space() const;
    template<Color Us> Score variant() const;
    Value winnable(Score score) const;

    const Position& pos;
    Material::Entry* me;
    Pawns::Entry* pe;
    Bitboard mobilityArea[COLOR_NB];
    Score mobility[COLOR_NB] = { SCORE_ZERO, SCORE_ZERO };
    Bitboard attackedBy[COLOR_NB][PIECE_TYPE_NB];
    Bitboard attackedBy2[COLOR_NB];
    Bitboard kingRing[COLOR_NB];
    int kingAttackersCount[COLOR_NB];
    int kingAttackersCountInHand[COLOR_NB];
    int kingAttackersWeight[COLOR_NB];
    int kingAttackersWeightInHand[COLOR_NB];
    int kingAttacksCount[COLOR_NB];
  };

  // URBINO: All Evaluation methods stubbed - Urbino uses district-based scoring
  // These are only called for chess/fairy variants during initialization

  template<Tracing T> template<Color Us>
  void Evaluation<T>::initialize() {
    // Minimal initialization
    attackedBy[Us][ALL_PIECES] = 0;
    attackedBy2[Us] = 0;
    mobilityArea[Us] = AllSquares;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::pieces(PieceType) {
    return SCORE_ZERO;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::hand(PieceType) {
    return SCORE_ZERO;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::king() const {
    return SCORE_ZERO;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::threats() const {
    return SCORE_ZERO;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::passed() const {
    return SCORE_ZERO;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::space() const {
    return SCORE_ZERO;
  }

  template<Tracing T> template<Color Us>
  Score Evaluation<T>::variant() const {
    return SCORE_ZERO;
  }

  template<Tracing T>
  Value Evaluation<T>::winnable(Score) const {
    return VALUE_ZERO;
  }

  template<Tracing T>
  Value Evaluation<T>::value() {
    // Minimal stubbed implementation for chess/fairy
    me = Material::probe(pos);
    pe = Pawns::probe(pos);
    
    if (me->specialized_eval_exists())
        return me->evaluate(pos);
    
    Score score = pos.psq_score();
    std::memset(attackedBy, 0, sizeof(attackedBy));
    initialize<WHITE>();
    initialize<BLACK>();
    
    Value v = mg_value(score);
    v = (v / 16) * 16;
    return pos.side_to_move() == WHITE ? v : -v;
  }

  Value fix_FRC(const Position&) {
    return VALUE_ZERO;  // Urbino doesn't use FRC
  }

} // namespace Eval

Value Eval::evaluate(const Position& pos) {

  Value v;

  // Urbino uses district-based scoring
  if (pos.urbino_gating()) {
      int white_score, black_score;
      pos.urbino_scores(white_score, black_score);

      // Add bonuses for pieces in hand (unplaced buildings)
      int white_palaces_in_hand = pos.count_in_hand(WHITE, CUSTOM_PIECE_3);
      int black_palaces_in_hand = pos.count_in_hand(BLACK, CUSTOM_PIECE_3);
      int white_towers_in_hand = pos.count_in_hand(WHITE, CUSTOM_PIECE_4);
      int black_towers_in_hand = pos.count_in_hand(BLACK, CUSTOM_PIECE_4);

      // Add to scores (multiply by 100 to maintain centipawn scale)
      int white_hand_bonus = 30 * white_palaces_in_hand + 100 * white_towers_in_hand;
      int black_hand_bonus = 30 * black_palaces_in_hand + 100 * black_towers_in_hand;
      
      /*
      // Phase-dependent tower bonus: starts at 250 early game, decreases to 0 at ply 15
      // Strategy: conserve towers early for late-game scoring swings
      int ply = pos.game_ply();
      int tower_bonus_per_tower = (ply >= 15) ? 0 : 250 * (15 - ply) / 15;

      // Add to scores (multiply by 100 to maintain centipawn scale)
      // Palace penalty removed - palaces rarely fully depleted in practice
      int white_hand_bonus = tower_bonus_per_tower * white_towers_in_hand;
      int black_hand_bonus = tower_bonus_per_tower * black_towers_in_hand;
      */
      v = Value((white_score - black_score) * 100 + white_hand_bonus - black_hand_bonus);
      return pos.side_to_move() == WHITE ? v : -v;
  }

  if (!Eval::useNNUE || !pos.nnue_applicable())
      v = Evaluation<NO_TRACE>(pos).value();
  else
  {
      // Scale and shift NNUE for compatibility with search and classical evaluation
      auto  adjusted_NNUE = [&]()
      {
         int scale =   903
                     + 32 * pos.count<PAWN>()
                     + 32 * pos.non_pawn_material() / 1024;

         Value nnue = NNUE::evaluate(pos, true) * scale / 1024;

         if (pos.is_chess960())
             nnue += fix_FRC(pos);

         if (pos.check_counting())
         {
             Color us = pos.side_to_move();
             nnue +=  6 * scale / (5 * pos.checks_remaining( us))
                    - 6 * scale / (5 * pos.checks_remaining(~us));
         }

         return nnue;
      };

      // If there is PSQ imbalance we use the classical eval, but we switch to
      // NNUE eval faster when shuffling or if the material on the board is high.
      int r50 = pos.rule50_count();
      Value psq = Value(abs(eg_value(pos.psq_score())));
      bool pure = !pos.check_counting();
      bool classical = psq * 5 > (750 + pos.non_pawn_material() / 64) * (5 + r50) && !pure;

      v = classical ? Evaluation<NO_TRACE>(pos).value()  // classical
                    : adjusted_NNUE();                   // NNUE
  }

  // Damp down the evaluation linearly when shuffling
  if (pos.n_move_rule())
  {
      v = v * (2 * pos.n_move_rule() - pos.rule50_count()) / (2 * pos.n_move_rule());
      if (pos.material_counting())
          v += pos.material_counting_result() / (10 * std::max(2 * pos.n_move_rule() - pos.rule50_count(), 1));
  }

  // Guarantee evaluation does not hit the virtual win/loss range
  if (pos.two_boards() && std::abs(v) >= VALUE_VIRTUAL_MATE_IN_MAX_PLY)
      v += v > VALUE_ZERO ? MAX_PLY + 1 : -MAX_PLY - 1;

  // Guarantee evaluation does not hit the tablebase range
  v = std::clamp(v, VALUE_TB_LOSS_IN_MAX_PLY + 1, VALUE_TB_WIN_IN_MAX_PLY - 1);

  return v;
}

/// trace() is like evaluate(), but instead of returning a value, it returns
/// a string (suitable for outputting to stdout) that contains the detailed
/// descriptions and values of each evaluation term. Useful for debugging.
/// Trace scores are from white's point of view

std::string Eval::trace(Position& pos) {

  if (pos.checkers())
      return "Final evaluation: none (in check)";

  std::stringstream ss;
  ss << std::showpoint << std::noshowpos << std::fixed << std::setprecision(2);

  Value v;

  std::memset(scores, 0, sizeof(scores));

  pos.this_thread()->trend = SCORE_ZERO; // Reset any dynamic contempt

  v = Evaluation<TRACE>(pos).value();

  ss << std::showpoint << std::noshowpos << std::fixed << std::setprecision(2)
     << " Contributing terms for the classical eval:\n"
     << "+------------+-------------+-------------+-------------+\n"
     << "|    Term    |    White    |    Black    |    Total    |\n"
     << "|            |   MG    EG  |   MG    EG  |   MG    EG  |\n"
     << "+------------+-------------+-------------+-------------+\n"
     << "|   Material | " << Term(MATERIAL)
     << "|  Imbalance | " << Term(IMBALANCE)
     << "|      Pawns | " << Term(PAWN)
     << "|    Knights | " << Term(KNIGHT)
     << "|    Bishops | " << Term(BISHOP)
     << "|      Rooks | " << Term(ROOK)
     << "|     Queens | " << Term(QUEEN)
     << "|   Mobility | " << Term(MOBILITY)
     << "|King safety | " << Term(KING)
     << "|    Threats | " << Term(THREAT)
     << "|     Passed | " << Term(PASSED)
     << "|      Space | " << Term(SPACE)
     << "|    Variant | " << Term(VARIANT)
     << "|   Winnable | " << Term(WINNABLE)
     << "+------------+-------------+-------------+-------------+\n"
     << "|      Total | " << Term(TOTAL)
     << "+------------+-------------+-------------+-------------+\n";

  if (Eval::useNNUE && pos.nnue_applicable())
      ss << '\n' << NNUE::trace(pos) << '\n';

  ss << std::showpoint << std::showpos << std::fixed << std::setprecision(2) << std::setw(15);

  v = pos.side_to_move() == WHITE ? v : -v;
  ss << "\nClassical evaluation   " << to_cp(v) << " (white side)\n";
  if (Eval::useNNUE && pos.nnue_applicable())
  {
      v = NNUE::evaluate(pos, false);
      v = pos.side_to_move() == WHITE ? v : -v;
      ss << "NNUE evaluation        " << to_cp(v) << " (white side)\n";
  }

  v = evaluate(pos);
  v = pos.side_to_move() == WHITE ? v : -v;
  ss << "Final evaluation       " << to_cp(v) << " (white side)";
  if (Eval::useNNUE && pos.nnue_applicable())
     ss << " [with scaled NNUE, hybrid, ...]";
  ss << "\n";

  return ss.str();
}

} // namespace Stockfish
