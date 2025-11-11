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

// URBINO: This file has been gutted - Urbino has no pawns, no kings.
// Kept minimal stubs for chess/fairy variant compatibility during initialization.

#include <algorithm>
#include <cassert>

#include "bitboard.h"
#include "pawns.h"
#include "position.h"
#include "thread.h"

namespace Stockfish {

namespace Pawns {

/// Pawns::probe() - Stubbed version for Urbino
/// Returns a minimal pawn entry since Urbino has no pawns
Entry* probe(const Position& pos) {
  Key key = pos.pawn_key();
  Entry* e = pos.this_thread()->pawnsTable[key];

  if (e->key == key)
      return e;

  e->key = key;
  e->blockedCount = 0;
  e->scores[WHITE] = SCORE_ZERO;
  e->scores[BLACK] = SCORE_ZERO;
  e->passedPawns[WHITE] = 0;
  e->passedPawns[BLACK] = 0;
  e->pawnAttacks[WHITE] = 0;
  e->pawnAttacks[BLACK] = 0;
  e->pawnAttacksSpan[WHITE] = 0;
  e->pawnAttacksSpan[BLACK] = 0;
  e->kingSquares[WHITE] = SQ_NONE;
  e->kingSquares[BLACK] = SQ_NONE;
  e->castlingRights[WHITE] = 0;
  e->castlingRights[BLACK] = 0;

  return e;
}

} // namespace Pawns

// Stub implementations for Entry member functions called by evaluate.cpp
// These are only used for chess/fairy variants, never for Urbino

template<Color Us>
Score Pawns::Entry::do_king_safety(const Position& pos) {
  return SCORE_ZERO;  // Urbino has no kings
}

template<Color Us>
Score Pawns::Entry::evaluate_shelter(const Position& pos, Square ksq) const {
  return SCORE_ZERO;  // Urbino has no kings or pawns
}

// Explicit template instantiations
template Score Pawns::Entry::do_king_safety<WHITE>(const Position& pos);
template Score Pawns::Entry::do_king_safety<BLACK>(const Position& pos);
template Score Pawns::Entry::evaluate_shelter<WHITE>(const Position& pos, Square ksq) const;
template Score Pawns::Entry::evaluate_shelter<BLACK>(const Position& pos, Square ksq) const;

} // namespace Stockfish
