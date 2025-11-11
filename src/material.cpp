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

// URBINO: This file has been gutted - Urbino uses district-based scoring, not material counting.
// Kept minimal stubs for chess/fairy variant compatibility during initialization.

#include <cassert>
#include <cstring>   // For std::memset

#include "material.h"
#include "thread.h"

using namespace std;

namespace Stockfish {

namespace Material {

/// Material::probe() - Stubbed version for Urbino
/// Returns a minimal material entry since Urbino doesn't use material-based eval
Entry* probe(const Position& pos) {

  Key key = pos.material_key(pos.endgame_eval());
  Entry* e = pos.this_thread()->materialTable[key];

  if (e->key == key)
      return e;

  std::memset(e, 0, sizeof(Entry));
  e->key = key;
  e->factor[WHITE] = e->factor[BLACK] = (uint8_t)SCALE_FACTOR_NORMAL;
  e->gamePhase = PHASE_MIDGAME;  // Default to midgame
  e->materialDensity = 0;

  // Urbino doesn't use specialized endgame functions
  e->evaluationFunction = nullptr;
  e->scalingFunction[WHITE] = nullptr;
  e->scalingFunction[BLACK] = nullptr;

  return e;
}

} // namespace Material

} // namespace Stockfish
