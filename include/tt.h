#ifndef YUUKI_TT_H
#define YUUKI_TT_H

#include "types.h"

namespace Yuuki {

void resizeTranspositionTable();
void clearTranspositionTable();
TTEntry* probeTranspositionTable(uint64_t hash, int depth, int alpha, int beta);
void storeTranspositionTable(uint64_t hash, int depth, int score, int flag, const Move& move);

}

#endif
