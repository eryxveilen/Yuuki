#include "tt.h"
#include "globals.h"

namespace Yuuki {

void resizeTranspositionTable() {
    size_t maxEntries = ((size_t)g_engineOptions.hashSizeMB * 1024 * 1024) / sizeof(TTEntry);
    g_ttSize = maxEntries;
    if (g_ttSize < 1024) g_ttSize = 1024;
    g_transpositionTable.assign(g_ttSize, TTEntry{});
    for (auto& e : g_transpositionTable) e.key = 0;
}

void clearTranspositionTable() {
    for (auto& e : g_transpositionTable) e.key = 0;
}

TTEntry* probeTranspositionTable(uint64_t hash, int depth, int alpha, int beta) {
    if (g_ttSize == 0) return nullptr;
    size_t idx = hash % g_ttSize;
    TTEntry* entry = &g_transpositionTable[idx];
    if (entry->key == hash && entry->depth >= depth) {
        g_searchStats.tthits++;
        if (entry->flag == TT_EXACT) {
            return entry;
        } else if (entry->flag == TT_ALPHA && entry->score <= alpha) {
            g_searchStats.ttcuts++;
            entry->score = alpha;
            return entry;
        } else if (entry->flag == TT_BETA && entry->score >= beta) {
            g_searchStats.ttcuts++;
            entry->score = beta;
            return entry;
        }
    }
    return nullptr;
}

void storeTranspositionTable(uint64_t hash, int depth, int score, int flag, const Move& move) {
    if (g_ttSize == 0) return;
    size_t idx = hash % g_ttSize;
    TTEntry* entry = &g_transpositionTable[idx];
    if (entry->key != 0 && entry->depth > depth + 2) return;
    entry->key = hash;
    entry->depth = depth;
    entry->score = score;
    entry->flag = flag;
    entry->moveFrom = move.from;
    entry->moveTo = move.to;
    entry->movePromotion = move.promotion;
    entry->age = g_fullMoveNumber;
}

}
