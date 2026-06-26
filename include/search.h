#ifndef YUUKI_SEARCH_H
#define YUUKI_SEARCH_H

#include "types.h"
#include <vector>

namespace Yuuki {

int quiescence(int ply, int alpha, int beta, int qDepth);
ABResult alphaBeta(int ply, int depth, int alpha, int beta, bool isPV, bool allowNull);
ABResult alphaBetaRoot(int depth, int alpha, int beta);
Move iterativeDeepening(int maxDepth, int timeLimit, bool infinite);

bool shouldStopTime(int timeLimit);
bool hasNonPawnMaterial(int color);

void scoreMoves(std::vector<Move>& moves, uint64_t hash, int ply);
void orderMoves(std::vector<Move>& moves, uint64_t hash, int ply);
void updateKillerMoves(const Move& move, int ply);
void updateHistory(const Move& move, int depth, int side);
void updateCounterMove(const Move& move, int side);

Move pickRandomMove(const std::vector<Move>& moves);
Move applySkillLevel(const Move& bestMove, const std::vector<Move>& allMoves, int depth);

void resetSearchState();
void buildLMRTable();
void buildFutilityMargins();
void buildRazorMargins();
void handleNewGame();

}

#endif
