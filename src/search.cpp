#include "search.h"
#include "globals.h"
#include "utils.h"
#include "move.h"
#include "attack.h"
#include "see.h"
#include "eval.h"
#include "tt.h"
#include "draw.h"
#include "uci.h"
#include "board.h"
#include <algorithm>
#include <random>
#include <numeric>
#include <ctime>
#include <cmath>

namespace Yuuki {

bool shouldStopTime(int timeLimit) {
    if (timeLimit <= 0) return false;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_searchStartTime).count();
    return elapsed >= timeLimit;
}

bool hasNonPawnMaterial(int color) {
    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_NONE && p.piece != PIECE_PAWN && p.piece != PIECE_KING && p.color == color) {
            return true;
        }
    }
    return false;
}

void scoreMoves(std::vector<Move>& moves, uint64_t hash, int ply) {
    for (auto& move : moves) {
        move.score = 0;

        if (move.captured != PIECE_NONE) {
            int seeVal = SEE_PIECE_VALUES[move.captured] * 10 - SEE_PIECE_VALUES[move.piece];
            move.score += 100000 + seeVal;
        }

        if (move.promotion != PIECE_NONE) {
            move.score += 90000 + getPieceValue(move.promotion);
        }

        if (g_ttSize > 0) {
            size_t idx = hash % g_ttSize;
            TTEntry* entry = &g_transpositionTable[idx];
            if (entry->key == hash && entry->moveFrom == move.from && entry->moveTo == move.to && entry->movePromotion == move.promotion) {
                move.score += 200000;
            }
        }

        if (ply < MAX_PLY) {
            if (g_killerMoves[ply][0].from != 0 || g_killerMoves[ply][0].to != 0) {
                if (movesEqual(move, g_killerMoves[ply][0])) move.score += 80000;
            }
            if (g_killerMoves[ply][1].from != 0 || g_killerMoves[ply][1].to != 0) {
                if (movesEqual(move, g_killerMoves[ply][1])) move.score += 70000;
            }
        }

        int historyScore = g_historyTable[g_sideToMove][move.from][move.to];
        if (historyScore) {
            move.score += std::min(historyScore, 60000);
        }

        Move counterMove{0, 0, 0, 0, 0, 0, 0};
        bool hasCounter = false;
        if (!g_moveHistory.empty()) {
            const Move& lastMove = g_moveHistory.back().move;
            if (lastMove.from != 0 || lastMove.to != 0) {
                if (g_counterMovesValid[1 - g_sideToMove][lastMove.from][lastMove.to]) {
                    counterMove = g_counterMoves[1 - g_sideToMove][lastMove.from][lastMove.to];
                    hasCounter = true;
                }
            }
        }
        if (hasCounter && movesEqual(move, counterMove)) {
            move.score += 65000;
        }
    }
}

void orderMoves(std::vector<Move>& moves, uint64_t hash, int ply) {
    scoreMoves(moves, hash, ply);
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

void updateKillerMoves(const Move& move, int ply) {
    if (move.captured != PIECE_NONE) return;
    if (ply >= MAX_PLY) return;
    if (!movesEqual(move, g_killerMoves[ply][0])) {
        g_killerMoves[ply][1] = g_killerMoves[ply][0];
        g_killerMoves[ply][0] = move;
    }
}

void updateHistory(const Move& move, int depth, int side) {
    if (move.captured != PIECE_NONE) return;
    int bonus = depth * depth;
    g_historyTable[side][move.from][move.to] += bonus;
    if (g_historyTable[side][move.from][move.to] > 1000000) {
        for (int i = 0; i < 128; i++) {
            for (int j = 0; j < 128; j++) {
                g_historyTable[side][i][j] /= 2;
            }
        }
    }
}

void updateCounterMove(const Move& move, int side) {
    if (g_moveHistory.empty()) return;
    const Move& lastMove = g_moveHistory.back().move;
    if (lastMove.from == 0 && lastMove.to == 0) return;
    g_counterMoves[side][lastMove.from][lastMove.to] = move;
    g_counterMovesValid[side][lastMove.from][lastMove.to] = true;
}

int quiescence(int ply, int alpha, int beta, int qDepth) {
    g_searchNodes++;
    g_searchStats.qnodes++;

    if (ply >= MAX_PLY - 1) return evaluate();
    if (qDepth > g_engineOptions.selectiveDepth) return evaluate();

    int standPat = evaluate();

    if (standPat >= beta) return beta;
    if (standPat > alpha) alpha = standPat;

    int delta = g_pieceValues.queen;
    if (standPat + delta < alpha) return alpha;

    auto moves = generateLegalMovesImpl(g_sideToMove);
    std::vector<Move> captures;
    for (const auto& move : moves) {
        if (move.captured != PIECE_NONE || move.promotion != PIECE_NONE || (move.flags & FLAG_ENPASSANT)) {
            captures.push_back(move);
        }
    }

    if (captures.empty()) {
        if (isInCheck(g_sideToMove)) {
            auto evasions = generateEvasions(g_sideToMove);
            if (evasions.empty()) return -VALUE_MATE + ply;

            for (const auto& move : evasions) {
                makeMove(move);
                int score = -quiescence(ply + 1, -beta, -alpha, qDepth + 1);
                undoMove();
                if (score >= beta) return beta;
                if (score > alpha) alpha = score;
            }
        }
        return alpha;
    }

    std::sort(captures.begin(), captures.end(), [](const Move& a, const Move& b) {
        int scoreA = getPieceValue(a.captured) * 10 - getPieceValue(a.piece);
        int scoreB = getPieceValue(b.captured) * 10 - getPieceValue(b.piece);
        if (a.promotion != PIECE_NONE) scoreA += getPieceValue(a.promotion) * 5;
        if (b.promotion != PIECE_NONE) scoreB += getPieceValue(b.promotion) * 5;
        return scoreA > scoreB;
    });

    for (const auto& move : captures) {
        if (move.captured != PIECE_NONE) {
            bool futile = standPat + getPieceValue(move.captured) + 200 < alpha;
            if (futile && move.promotion == PIECE_NONE && !isInCheck(1 - g_sideToMove)) {
                continue;
            }
        }

        makeMove(move);
        int score = -quiescence(ply + 1, -beta, -alpha, qDepth + 1);
        undoMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

ABResult alphaBeta(int ply, int depth, int alpha, int beta, bool isPV, bool allowNull) {
    g_searchNodes++;
    g_searchStats.nodes++;
    if (ply > g_selDepth) g_selDepth = ply;

    if (ply >= MAX_PLY - 1) {
        return {evaluate(), Move{0, 0, 0, 0, 0, 0, 0}, {}};
    }

    if (g_searchNodes % 1024 == 0) {
        if (g_stopSearch.load()) return {alpha, Move{0, 0, 0, 0, 0, 0, 0}, {}};
    }

    if (isDraw()) {
        return {g_engineOptions.drawScore, Move{0, 0, 0, 0, 0, 0, 0}, {}};
    }

    uint64_t hash = getPositionHash();

    if (!isPV && !g_moveHistory.empty()) {
        TTEntry* ttEntry = probeTranspositionTable(hash, depth, alpha, beta);
        if (ttEntry) {
            Move ttMove{ttEntry->moveFrom, ttEntry->moveTo, 0, 0, ttEntry->movePromotion, 0, 0};
            return {ttEntry->score, ttMove, {ttMove}};
        }
    }

    bool inCheck = isInCheck(g_sideToMove);

    if (inCheck) {
        depth++;
        g_searchStats.checkExtensions++;
    }

    if (depth <= 0) {
        int qScore = quiescence(ply, alpha, beta, 0);
        return {qScore, Move{0, 0, 0, 0, 0, 0, 0}, {}};
    }

    auto moves = generateLegalMovesImpl(g_sideToMove);
    if (moves.empty()) {
        if (inCheck) return {-VALUE_MATE + ply, Move{0, 0, 0, 0, 0, 0, 0}, {}};
        return {g_engineOptions.drawScore, Move{0, 0, 0, 0, 0, 0, 0}, {}};
    }

    if (moves.size() == 1 && ply > 0) {
        depth++;
        g_searchStats.extensions++;
    }

    int staticEval = evaluate();
    g_searchStack[ply].staticEval = staticEval;

    if (!isPV && !inCheck && allowNull && depth >= g_engineOptions.nullMoveDepthLimit) {
        if (staticEval >= beta && hasNonPawnMaterial(g_sideToMove)) {
            int nullR = g_engineOptions.nullMoveReduction;
            if (depth > 6) nullR = 4;
            int nullDepth = depth - nullR - 1;

            if (nullDepth > 0) {
                makeNullMove();
                auto nullResult = alphaBeta(ply + 1, nullDepth, -beta, -beta + 1, false, false);
                undoNullMove();
                int nullScore = -nullResult.score;

                if (nullScore >= beta) {
                    g_searchStats.nullCuts++;
                    if (nullScore >= VALUE_MATE - MAX_PLY) nullScore = beta;
                    return {beta, Move{0, 0, 0, 0, 0, 0, 0}, {}};
                }
            }
        }
    }

    if (!isPV && !inCheck) {
        if (depth <= g_engineOptions.razorDepthLimit && std::abs(beta) < VALUE_MATE - MAX_PLY) {
            int razorMargin = (depth < 4) ? RAZOR_MARGIN_TABLE[depth] : 300;
            if (staticEval + razorMargin < beta) {
                int qScore = quiescence(ply, alpha, beta, 0);
                g_searchStats.razorPrunes++;
                if (qScore < beta) {
                    return {qScore, Move{0, 0, 0, 0, 0, 0, 0}, {}};
                }
            }
        }

        if (depth <= g_engineOptions.futilityDepthLimit && std::abs(alpha) < VALUE_MATE - MAX_PLY) {
            int futilityMargin = (depth < 4) ? FUTILITY_MARGIN_TABLE[depth] : g_engineOptions.futilityMargin * depth;
            if (staticEval - futilityMargin >= beta && hasNonPawnMaterial(g_sideToMove)) {
                g_searchStats.futilityPrunes++;
                return {beta, Move{0, 0, 0, 0, 0, 0, 0}, {}};
            }
        }
    }

    orderMoves(moves, hash, ply);

    int bestScore = -VALUE_INFINITE;
    Move bestMove{0, 0, 0, 0, 0, 0, 0};
    std::vector<Move> bestPV;
    int flag = TT_ALPHA;
    int movesSearched = 0;

    for (size_t i = 0; i < moves.size(); i++) {
        const Move& move = moves[i];
        makeMove(move);
        g_searchStack[ply].currentMove = move;
        g_searchStack[ply].moveCount = movesSearched;

        int score;
        int newDepth = depth - 1;

        if (!isPV && !inCheck && depth >= g_engineOptions.lmrDepthThreshold &&
            movesSearched >= g_engineOptions.lmrMoveCountThreshold &&
            move.captured == PIECE_NONE && move.promotion == PIECE_NONE) {
            int reduction = LMR_REDUCTION_TABLE[std::min(depth, 63)][std::min(movesSearched, 63)];
            if (isPV) reduction = std::max(0, reduction - 1);

            if (reduction > 0) {
                auto lmrResult = alphaBeta(ply + 1, newDepth - reduction, -alpha - 1, -alpha, false, true);
                score = -lmrResult.score;
                if (score > alpha) {
                    lmrResult = alphaBeta(ply + 1, newDepth, -alpha - 1, -alpha, false, true);
                    score = -lmrResult.score;
                }
                g_searchStats.lmrReductions++;
            } else {
                auto result = alphaBeta(ply + 1, newDepth, -beta, -alpha, isPV, true);
                score = -result.score;
            }
        } else {
            if (i == 0) {
                auto result = alphaBeta(ply + 1, newDepth, -beta, -alpha, isPV, true);
                score = -result.score;
            } else {
                auto result = alphaBeta(ply + 1, newDepth, -alpha - 1, -alpha, false, true);
                score = -result.score;
                if (score > alpha && score < beta) {
                    result = alphaBeta(ply + 1, newDepth, -beta, -alpha, true, true);
                    score = -result.score;
                }
            }
        }

        undoMove();
        movesSearched++;

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
            bestPV = {move};
            if (ply + 1 < MAX_PLY && !g_searchStack[ply + 1].pv.empty()) {
                bestPV.insert(bestPV.end(), g_searchStack[ply + 1].pv.begin(), g_searchStack[ply + 1].pv.end());
            }

            if (score > alpha) {
                alpha = score;
                flag = TT_EXACT;
                g_searchStack[ply].pv = {move};
                if (ply + 1 < MAX_PLY && !g_searchStack[ply + 1].pv.empty()) {
                    g_searchStack[ply].pv.insert(g_searchStack[ply].pv.end(), g_searchStack[ply + 1].pv.begin(), g_searchStack[ply + 1].pv.end());
                }

                if (score >= beta) {
                    flag = TT_BETA;
                    if (move.captured == PIECE_NONE) {
                        updateKillerMoves(move, ply);
                        updateHistory(move, depth, g_sideToMove);
                        updateCounterMove(move, g_sideToMove);
                    }
                    storeTranspositionTable(hash, depth, bestScore, flag, move);
                    return {bestScore, move, {move}};
                }
            }
        }
    }

    storeTranspositionTable(hash, depth, bestScore, flag, bestMove);
    return {bestScore, bestMove, bestPV.empty() ? g_searchStack[ply].pv : bestPV};
}

ABResult alphaBetaRoot(int depth, int alpha, int beta) {
    uint64_t hash = getPositionHash();
    bool inCheck = isInCheck(g_sideToMove);
    if (inCheck) depth++;

    auto moves = generateLegalMovesImpl(g_sideToMove);
    if (moves.empty()) {
        if (inCheck) return {-VALUE_MATE, Move{0, 0, 0, 0, 0, 0, 0}, {}};
        return {g_engineOptions.drawScore, Move{0, 0, 0, 0, 0, 0, 0}, {}};
    }

    orderMoves(moves, hash, 0);

    int bestScore = -VALUE_INFINITE;
    Move bestMove{0, 0, 0, 0, 0, 0, 0};
    std::vector<Move> bestPV;

    for (size_t i = 0; i < moves.size(); i++) {
        if (g_stopSearch.load()) break;

        const Move& move = moves[i];
        makeMove(move);
        g_searchStack[0].currentMove = move;

        int score;
        if (i == 0) {
            auto result = alphaBeta(1, depth - 1, -beta, -alpha, true, true);
            score = -result.score;
        } else {
            int lmrDepth = depth - 1;
            bool doFullSearch = true;

            if (depth >= g_engineOptions.lmrDepthThreshold &&
                static_cast<int>(i) >= g_engineOptions.lmrMoveCountThreshold &&
                !inCheck && move.captured == PIECE_NONE && move.promotion == PIECE_NONE) {
                int reduction = LMR_REDUCTION_TABLE[std::min(depth, 63)][std::min(static_cast<int>(i), 63)];
                if (reduction > 0) {
                    auto result = alphaBeta(1, lmrDepth - reduction, -alpha - 1, -alpha, false, false);
                    score = -result.score;
                    if (score <= alpha) {
                        doFullSearch = false;
                    }
                    g_searchStats.lmrReductions++;
                }
            }

            if (doFullSearch) {
                auto result = alphaBeta(1, lmrDepth, -alpha - 1, -alpha, false, false);
                score = -result.score;
                if (score > alpha && score < beta) {
                    result = alphaBeta(1, lmrDepth, -beta, -alpha, true, true);
                    score = -result.score;
                }
            }
        }

        undoMove();
        g_searchNodes++;
        g_searchStats.nodes++;

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
            bestPV = {move};
            if (!g_searchStack[1].pv.empty()) {
                bestPV.insert(bestPV.end(), g_searchStack[1].pv.begin(), g_searchStack[1].pv.end());
            }

            if (score > alpha) {
                alpha = score;
                if (score >= beta) {
                    updateKillerMoves(move, 0);
                    updateHistory(move, depth, g_sideToMove);
                    updateCounterMove(move, g_sideToMove);
                    storeTranspositionTable(hash, depth, beta, TT_BETA, move);
                    return {beta, move, {move}};
                }
            }
        }
    }

    if (bestMove.from != 0 || bestMove.to != 0) {
        int flag = (bestScore >= beta) ? TT_BETA : ((bestScore > alpha) ? TT_EXACT : TT_ALPHA);
        storeTranspositionTable(hash, depth, bestScore, flag, bestMove);
    }

    return {bestScore, bestMove, bestPV};
}

Move iterativeDeepening(int maxDepth, int timeLimit, bool infinite) {
    g_stopSearch = false;
    g_searchNodes = 0;
    g_searchStartTime = std::chrono::steady_clock::now();
    g_isSearching = true;
    g_bestMoveChanges = 0;
    g_stableBestMoveCount = 0;
    g_searchStats = SearchStats{};

    for (int i = 0; i < MAX_PLY; i++) {
        g_killerMoves[i] = {Move{0, 0, 0, 0, 0, 0, 0}, Move{0, 0, 0, 0, 0, 0, 0}};
        g_searchStack[i] = SearchStackEntry{};
    }

    if (g_engineOptions.cleanSearch) {
        std::memset(g_historyTable, 0, sizeof(g_historyTable));
        std::memset(g_counterMoves, 0, sizeof(g_counterMoves));
        std::memset(g_counterMovesValid, 0, sizeof(g_counterMovesValid));
    }

    Move bestMove{0, 0, 0, 0, 0, 0, 0};
    int bestScore = 0;
    int aspirationWindow = g_engineOptions.aspirationWindow;
    int aspirationMinDepth = g_engineOptions.aspirationWindowMinDepth;

    for (int depth = 1; depth <= maxDepth; depth++) {
        if (g_stopSearch.load() && !infinite) break;
        if (shouldStopTime(timeLimit) && !infinite && depth > 1) break;

        g_currentDepth = depth;
        g_selDepth = 0;

        int alpha = -VALUE_INFINITE;
        int beta = VALUE_INFINITE;

        if (depth >= aspirationMinDepth) {
            alpha = bestScore - aspirationWindow;
            beta = bestScore + aspirationWindow;
        }

        auto result = alphaBetaRoot(depth, alpha, beta);

        if (g_stopSearch.load() && !infinite && (result.move.from == 0 && result.move.to == 0)) break;

        if (result.score <= alpha || result.score >= beta) {
            if (!g_stopSearch.load() || infinite) {
                g_searchStats.aspirationResearches++;
                alpha = -VALUE_INFINITE;
                beta = VALUE_INFINITE;
                result = alphaBetaRoot(depth, alpha, beta);
            }
        }

        if (g_stopSearch.load() && !infinite && (result.move.from == 0 && result.move.to == 0)) break;

        if (result.move.from != 0 || result.move.to != 0) {
            bestMove = result.move;
            bestScore = result.score;
            g_currentPV = result.pv.empty() ? std::vector<Move>{bestMove} : result.pv;
            g_currentScore = bestScore;

            if (!movesEqual(bestMove, g_previousBestMove)) {
                g_bestMoveChanges++;
                g_stableBestMoveCount = 0;
                g_previousBestMove = bestMove;
            } else {
                g_stableBestMoveCount++;
            }

            auto now = std::chrono::steady_clock::now();
            int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - g_searchStartTime).count());
            uint64_t nps = elapsed > 0 ? (g_searchNodes / (elapsed / 1000.0)) : 0;
            g_searchStats.nps = static_cast<uint64_t>(nps);

            std::string scoreStr;
            bool isMate = false;
            if (std::abs(bestScore) > VALUE_MATE - MAX_PLY) {
                int mateDist = bestScore > 0 ? static_cast<int>((VALUE_MATE - bestScore + 1) / 2) : -static_cast<int>((VALUE_MATE + bestScore) / 2);
                scoreStr = "mate " + std::to_string(mateDist);
                isMate = true;
            } else {
                scoreStr = "cp " + std::to_string(bestScore);
            }

            std::string pvStr;
            for (size_t j = 0; j < g_currentPV.size(); j++) {
                if (j > 0) pvStr += " ";
                pvStr += moveToString(g_currentPV[j]);
            }
            if (pvStr.empty()) pvStr = moveToString(bestMove);

            std::string infoStr = "depth " + std::to_string(depth) + " seldepth " + std::to_string(g_selDepth) +
                " score " + scoreStr + " nodes " + std::to_string(g_searchNodes) + " time " + std::to_string(elapsed) +
                " nps " + std::to_string(static_cast<uint64_t>(nps));
            if (!pvStr.empty()) {
                infoStr += " pv " + pvStr;
            }
            sendInfo(infoStr);

            if (g_engineOptions.verboseUCI) {
                std::string debugStr = "Depth " + std::to_string(depth) + ": " +
                    (isMate ? "M" + std::to_string(static_cast<int>((VALUE_MATE - std::abs(bestScore)) / 2)) : std::to_string(bestScore)) +
                    " | " + moveToString(bestMove) + " | " + pvStr;
                sendInfoString(debugStr);
            }
        }

        if (!infinite && timeLimit > 0) {
            auto now = std::chrono::steady_clock::now();
            int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - g_searchStartTime).count());
            double timeUsage = static_cast<double>(elapsed) / timeLimit;
            if (g_stableBestMoveCount >= 3 && depth >= 8 && timeUsage > 0.5) break;
            if (g_stableBestMoveCount >= 5 && depth >= 12 && timeUsage > 0.3) break;
        }
    }

    g_lastBestMove = bestMove;
    return bestMove;
}

Move pickRandomMove(const std::vector<Move>& moves) {
    if (moves.empty()) return Move{0, 0, 0, 0, 0, 0, 0};
    std::vector<int> weights;
    for (const auto& m : moves) {
        weights.push_back(m.score > 0 ? m.score : (moves.size() - (&m - moves.data())));
    }
    int total = std::accumulate(weights.begin(), weights.end(), 0);
    if (total <= 0) return moves[0];
    static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, total - 1);
    int random = dist(rng);
    int cumulative = 0;
    for (size_t i = 0; i < moves.size(); i++) {
        cumulative += weights[i];
        if (random < cumulative) return moves[i];
    }
    return moves[0];
}

Move applySkillLevel(const Move& bestMove, const std::vector<Move>& allMoves, int depth) {
    if (g_engineOptions.skillLevel >= 20) return bestMove;
    if ((bestMove.from == 0 && bestMove.to == 0) || allMoves.size() <= 1) return bestMove;

    double probability = g_engineOptions.skillLevel / 20.0;
    double blunderChance = (1 - probability) * (1 - probability);

    static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    if (dist(rng) < blunderChance && allMoves.size() > 1) {
        auto weakMoves = allMoves;
        weakMoves.erase(weakMoves.begin());
        size_t idx = std::uniform_int_distribution<size_t>(0, std::min(weakMoves.size(), size_t(3)) - 1)(rng);
        return weakMoves[idx];
    }

    if (dist(rng) > probability && allMoves.size() > 1) {
        size_t idx = std::uniform_int_distribution<size_t>(0, std::min(allMoves.size(), size_t(2)) - 1)(rng);
        return allMoves[idx];
    }

    return bestMove;
}

void resetSearchState() {
    clearTranspositionTable();
    g_evalCache.clear();
    g_pawnHashTable.clear();
    std::memset(g_historyTable, 0, sizeof(g_historyTable));
    std::memset(g_counterMoves, 0, sizeof(g_counterMoves));
    std::memset(g_counterMovesValid, 0, sizeof(g_counterMovesValid));
    for (int i = 0; i < MAX_PLY; i++) {
        g_killerMoves[i] = {Move{0, 0, 0, 0, 0, 0, 0}, Move{0, 0, 0, 0, 0, 0, 0}};
        g_searchStack[i] = SearchStackEntry{};
    }
    g_searchNodes = 0;
    g_stopSearch = false;
    g_isSearching = false;
    g_bestMoveChanges = 0;
    g_previousBestMove = Move{0, 0, 0, 0, 0, 0, 0};
    g_stableBestMoveCount = 0;
    g_searchStats = SearchStats{};
}

void buildLMRTable() {
    double base = g_engineOptions.lmrBaseReduction;
    for (int depth = 1; depth < 64; depth++) {
        for (int moveCount = 1; moveCount < 64; moveCount++) {
            int reduction = static_cast<int>(base + std::log(depth) * std::log(moveCount) / 2.25);
            if (reduction < 0) reduction = 0;
            if (reduction > depth - 1) reduction = depth - 1;
            LMR_REDUCTION_TABLE[depth][moveCount] = reduction;
        }
    }
}

void buildFutilityMargins() {
    int base = g_engineOptions.futilityMargin;
    FUTILITY_MARGIN_TABLE[0] = 0;
    for (int i = 1; i < 10; i++) {
        FUTILITY_MARGIN_TABLE[i] = base * i;
    }
}

void buildRazorMargins() {
    int base = g_engineOptions.razorMargin;
    RAZOR_MARGIN_TABLE[0] = 0;
    RAZOR_MARGIN_TABLE[1] = base;
    RAZOR_MARGIN_TABLE[2] = base + 150;
    RAZOR_MARGIN_TABLE[3] = base + 300;
}

void handleNewGame() {
    resetSearchState();
    initializeBoard();
    initZobrist();
    resizeTranspositionTable();
    clearEvaluationCache();
    clearPawnHashTable();
    g_positionHistory.clear();
    g_moveHistory.clear();
    g_lastBestMove = Move{0, 0, 0, 0, 0, 0, 0};
    g_previousBestMove = Move{0, 0, 0, 0, 0, 0, 0};
    g_bestMoveChanges = 0;
    g_stableBestMoveCount = 0;
}

}
