#include "eval.h"
#include "globals.h"
#include "utils.h"
#include "pstables.h"
#include "attack.h"
#include <algorithm>

namespace Yuuki {

void clearEvaluationCache() {
    g_evalCache.clear();
}

void clearPawnHashTable() {
    g_pawnHashTable.clear();
}

int evaluateMaterial() {
    int score = 0;
    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE) continue;
        int val = getPieceValue(p.piece);
        if (p.color == COLOR_WHITE) score += val;
        else score -= val;
    }
    return score;
}

int evaluatePST() {
    int score = 0;
    bool isEG = (g_gamePhase == GAME_PHASE_ENDGAME);

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE) continue;

        int sq = (p.color == COLOR_WHITE) ? i : getBlackPSTIndex(i);
        int pstVal = getPSTValue(p.piece, sq, isEG);

        if (p.color == COLOR_WHITE) score += pstVal;
        else score -= pstVal;
    }

    return score;
}

int evaluateMobility() {
    int score = 0;

    for (int color = 0; color < 2; color++) {
        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i)) continue;
            const Piece& p = g_board[i];
            if (p.piece == PIECE_NONE || p.color != color) continue;

            int mobility = 0;
            if (p.piece == PIECE_KNIGHT) {
                for (int j = 0; j < 8; j++) {
                    int to = i + KNIGHT_DELTAS[j];
                    if (isValidSquare(to) && (g_board[to].piece == PIECE_NONE || g_board[to].color != color)) mobility++;
                }
                int bonus = MOBILITY_BONUS_KNIGHT[std::min(mobility, 8)];
                score += (color == COLOR_WHITE ? bonus : -bonus) * g_engineOptions.mobilityWeight / 8;
            } else if (p.piece == PIECE_BISHOP) {
                for (int j = 0; j < 4; j++) {
                    int to = i + BISHOP_DELTAS[j];
                    while (isValidSquare(to)) {
                        if (g_board[to].piece == PIECE_NONE) {
                            mobility++;
                        } else {
                            if (g_board[to].color != color) mobility++;
                            break;
                        }
                        to += BISHOP_DELTAS[j];
                    }
                }
                int bonus = MOBILITY_BONUS_BISHOP[std::min(mobility, 13)];
                score += (color == COLOR_WHITE ? bonus : -bonus) * g_engineOptions.mobilityWeight / 8;
            } else if (p.piece == PIECE_ROOK) {
                for (int j = 0; j < 4; j++) {
                    int to = i + ROOK_DELTAS[j];
                    while (isValidSquare(to)) {
                        if (g_board[to].piece == PIECE_NONE) {
                            mobility++;
                        } else {
                            if (g_board[to].color != color) mobility++;
                            break;
                        }
                        to += ROOK_DELTAS[j];
                    }
                }
                int bonus = MOBILITY_BONUS_ROOK[std::min(mobility, 14)];
                score += (color == COLOR_WHITE ? bonus : -bonus) * g_engineOptions.mobilityWeight / 8;
            } else if (p.piece == PIECE_QUEEN) {
                for (int j = 0; j < 8; j++) {
                    int to = i + QUEEN_DELTAS[j];
                    while (isValidSquare(to)) {
                        if (g_board[to].piece == PIECE_NONE) {
                            mobility++;
                        } else {
                            if (g_board[to].color != color) mobility++;
                            break;
                        }
                        to += QUEEN_DELTAS[j];
                    }
                }
                int bonus = MOBILITY_BONUS_QUEEN[std::min(mobility, 27)];
                score += (color == COLOR_WHITE ? bonus : -bonus) * g_engineOptions.mobilityWeight / 8;
            }
        }
    }

    return score;
}

int evaluatePawnStructure() {
    uint64_t pawnHash = getPawnHash();
    auto it = g_pawnHashTable.find(pawnHash);
    if (it != g_pawnHashTable.end()) {
        return it->second;
    }

    int score = 0;
    int whitePawnsByFile[8] = {0};
    int blackPawnsByFile[8] = {0};
    int whitePawnRanks[8] = {0};
    int blackPawnRanks[8] = {0};

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_PAWN) continue;
        int f = squareFile(i);
        int r = squareRank(i);
        if (p.color == COLOR_WHITE) {
            whitePawnsByFile[f]++;
            if (whitePawnRanks[f] == 0 || r > whitePawnRanks[f]) whitePawnRanks[f] = r;
        } else {
            blackPawnsByFile[f]++;
            if (blackPawnRanks[f] == 0 || r < blackPawnRanks[f]) blackPawnRanks[f] = r;
        }
    }

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_PAWN) continue;
        int f = squareFile(i);
        int r = squareRank(i);
        bool isWhite = (p.color == COLOR_WHITE);

        if (isWhite) {
            if (whitePawnsByFile[f] > 1) {
                score -= g_engineOptions.doubledPawnPenalty * (whitePawnsByFile[f] - 1);
            }
        } else {
            if (blackPawnsByFile[f] > 1) {
                score += g_engineOptions.doubledPawnPenalty * (blackPawnsByFile[f] - 1);
            }
        }

        bool hasNeighbor = false;
        if (f > 0) {
            if (isWhite && whitePawnsByFile[f - 1] > 0) hasNeighbor = true;
            if (!isWhite && blackPawnsByFile[f - 1] > 0) hasNeighbor = true;
        }
        if (f < 7) {
            if (isWhite && whitePawnsByFile[f + 1] > 0) hasNeighbor = true;
            if (!isWhite && blackPawnsByFile[f + 1] > 0) hasNeighbor = true;
        }

        if (!hasNeighbor) {
            if (isWhite) score -= g_engineOptions.isolatedPawnPenalty;
            else score += g_engineOptions.isolatedPawnPenalty;
        }

        bool isBackward = true;
        if (f > 0) {
            int checkRank = isWhite ? r + 1 : r - 1;
            if (isWhite && whitePawnRanks[f - 1] > checkRank) isBackward = false;
            if (!isWhite && blackPawnRanks[f - 1] < checkRank) isBackward = false;
        }
        if (f < 7) {
            int checkRank = isWhite ? r + 1 : r - 1;
            if (isWhite && whitePawnRanks[f + 1] > checkRank) isBackward = false;
            if (!isWhite && blackPawnRanks[f + 1] < checkRank) isBackward = false;
        }
        if (isBackward && !hasNeighbor) {
            if (isWhite) score -= g_engineOptions.backwardPawnPenalty;
            else score += g_engineOptions.backwardPawnPenalty;
        }

        bool isPassed = true;
        if (isWhite) {
            for (int rf = std::max(0, f - 1); rf <= std::min(7, f + 1); rf++) {
                if (blackPawnsByFile[rf] > 0 && blackPawnRanks[rf] < r) {
                    isPassed = false;
                    break;
                }
            }
        } else {
            for (int rf = std::max(0, f - 1); rf <= std::min(7, f + 1); rf++) {
                if (whitePawnsByFile[rf] > 0 && whitePawnRanks[rf] > r) {
                    isPassed = false;
                    break;
                }
            }
        }

        if (isPassed) {
            int dist = isWhite ? r : 7 - r;
            int bonus = PASSED_PAWN_BONUS_MG[dist] * (24 - g_phaseScore) / 24 +
                       PASSED_PAWN_BONUS_EG[dist] * g_phaseScore / 24;
            if (isWhite) score += bonus * g_engineOptions.passedPawnWeight / 50;
            else score -= bonus * g_engineOptions.passedPawnWeight / 50;
        }

        bool isCandidate = !isPassed;
        if (isCandidate) {
            int blockers = 0;
            if (isWhite) {
                for (int rf = std::max(0, f - 1); rf <= std::min(7, f + 1); rf++) {
                    if (blackPawnsByFile[rf] > 0) blockers++;
                }
            } else {
                for (int rf = std::max(0, f - 1); rf <= std::min(7, f + 1); rf++) {
                    if (whitePawnsByFile[rf] > 0) blockers++;
                }
            }
            if (blockers == 1) {
                int dist = isWhite ? r : 7 - r;
                if (isWhite) score += CANDIDATE_PASSED_PAWN_BONUS[dist] * g_engineOptions.passedPawnWeight / 100;
                else score -= CANDIDATE_PASSED_PAWN_BONUS[dist] * g_engineOptions.passedPawnWeight / 100;
            }
        }

        if (isWhite && r >= 4) score += g_engineOptions.advancedPawnBonus * (r - 3) / 4;
        if (!isWhite && r <= 3) score -= g_engineOptions.advancedPawnBonus * (4 - r) / 4;

        if (f >= 2 && f <= 5) {
            if (isWhite && r >= 2 && r <= 4) score += g_engineOptions.centralPawnBonus;
            if (!isWhite && r >= 3 && r <= 5) score -= g_engineOptions.centralPawnBonus;
        }

        if (f >= 3 && f <= 4 && r >= 3 && r <= 4) {
            if (isWhite) score += g_engineOptions.centralPawnBonus;
            else score -= g_engineOptions.centralPawnBonus;
        }

        bool isChained = false;
        if (f > 0) {
            int adjSq = makeSquare(f - 1, isWhite ? r - 1 : r + 1);
            if (isValidSquare(adjSq) && g_board[adjSq].piece == PIECE_PAWN && g_board[adjSq].color == p.color) isChained = true;
        }
        if (f < 7) {
            int adjSq = makeSquare(f + 1, isWhite ? r - 1 : r + 1);
            if (isValidSquare(adjSq) && g_board[adjSq].piece == PIECE_PAWN && g_board[adjSq].color == p.color) isChained = true;
        }
        if (isChained) {
            if (isWhite) score += g_engineOptions.pawnChainBonus;
            else score -= g_engineOptions.pawnChainBonus;
        }

        if (f <= 1 || f >= 6) {
            if (r >= 4 && isWhite) score -= g_engineOptions.overextendedPawnPenalty;
            if (r <= 3 && !isWhite) score += g_engineOptions.overextendedPawnPenalty;
        }
    }

    int result = score * g_engineOptions.pawnStructureWeight / 12;
    g_pawnHashTable[pawnHash] = result;
    return result;
}

int evaluatePawnShield(int kingSq, int color) {
    int score = 0;
    int kf = squareFile(kingSq);

    int shieldRanks[2] = {color == COLOR_WHITE ? 1 : 6, color == COLOR_WHITE ? 2 : 5};

    for (int i = 0; i < 2; i++) {
        int sr = shieldRanks[i];
        int weight = (i == 0) ? 3 : 1;
        for (int df = -1; df <= 1; df++) {
            int f = kf + df;
            if (f < 0 || f > 7) continue;
            int sq = makeSquare(f, sr);
            if (g_board[sq].piece == PIECE_PAWN && g_board[sq].color == color) {
                score += 5 * weight;
            }
        }
    }

    return score;
}

int evaluatePawnStorm(int kingSq, int color) {
    int score = 0;
    int kf = squareFile(kingSq);
    int enemyColor = 1 - color;
    int stormRanks[3] = {color == COLOR_WHITE ? 3 : 2, color == COLOR_WHITE ? 4 : 3, color == COLOR_WHITE ? 5 : 4};

    for (int df = -1; df <= 1; df++) {
        int f = kf + df;
        if (f < 0 || f > 7) continue;
        for (int i = 0; i < 3; i++) {
            int sq = makeSquare(f, stormRanks[i]);
            if (g_board[sq].piece == PIECE_PAWN && g_board[sq].color == enemyColor) {
                score -= 3 * (3 - i);
            }
        }
    }

    return score;
}

int evaluateKingSafety() {
    int score = 0;
    int wKingSq = findKingSquare(COLOR_WHITE);
    int bKingSq = findKingSquare(COLOR_BLACK);

    if (wKingSq == -1 || bKingSq == -1) return 0;

    int wKingFile = squareFile(wKingSq);
    int wKingRank = squareRank(wKingSq);
    int bKingFile = squareFile(bKingSq);
    int bKingRank = squareRank(bKingSq);

    int wAttacks = 0, wAttackers = 0;
    int bAttacks = 0, bAttackers = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE || p.piece == PIECE_PAWN || p.piece == PIECE_KING) continue;

        int f = squareFile(i);
        int r = squareRank(i);

        if (p.color == COLOR_BLACK) {
            int distToWK = std::max(std::abs(f - wKingFile), std::abs(r - wKingRank));
            if (distToWK <= 3) {
                bAttacks += getPieceValue(p.piece) / 200;
                bAttackers++;
            }
        }

        if (p.color == COLOR_WHITE) {
            int distToBK = std::max(std::abs(f - bKingFile), std::abs(r - bKingRank));
            if (distToBK <= 3) {
                wAttacks += getPieceValue(p.piece) / 200;
                wAttackers++;
            }
        }
    }

    int wAttackIndex = std::min(static_cast<int>(bAttacks + bAttackers), 10);
    int bAttackIndex = std::min(static_cast<int>(wAttacks + wAttackers), 10);

    score -= KING_SAFETY_ATTACK_VALUE[wAttackIndex] * g_engineOptions.kingSafetyWeight / 100;
    score += KING_SAFETY_ATTACK_VALUE[bAttackIndex] * g_engineOptions.kingSafetyWeight / 100;

    score += evaluatePawnShield(wKingSq, COLOR_WHITE) * g_engineOptions.pawnShieldWeight / 20;
    score -= evaluatePawnShield(bKingSq, COLOR_BLACK) * g_engineOptions.pawnShieldWeight / 20;

    score += evaluatePawnStorm(wKingSq, COLOR_WHITE) * g_engineOptions.pawnStormWeight / 20;
    score -= evaluatePawnStorm(bKingSq, COLOR_BLACK) * g_engineOptions.pawnStormWeight / 20;

    int wOpenFiles = 0, bOpenFiles = 0;
    for (int f = std::max(0, wKingFile - 1); f <= std::min(7, wKingFile + 1); f++) {
        bool hasWhitePawn = false, hasBlackPawn = false;
        for (int r = 0; r < 8; r++) {
            int sq = makeSquare(f, r);
            if (g_board[sq].piece == PIECE_PAWN) {
                if (g_board[sq].color == COLOR_WHITE) hasWhitePawn = true;
                else hasBlackPawn = true;
            }
        }
        if (!hasWhitePawn && !hasBlackPawn) wOpenFiles++;
        if (!hasWhitePawn && hasBlackPawn) wOpenFiles += 1;
    }
    for (int f = std::max(0, bKingFile - 1); f <= std::min(7, bKingFile + 1); f++) {
        bool hasWhitePawn = false, hasBlackPawn = false;
        for (int r = 0; r < 8; r++) {
            int sq = makeSquare(f, r);
            if (g_board[sq].piece == PIECE_PAWN) {
                if (g_board[sq].color == COLOR_WHITE) hasWhitePawn = true;
                else hasBlackPawn = true;
            }
        }
        if (!hasWhitePawn && !hasBlackPawn) bOpenFiles++;
        if (hasWhitePawn && !hasBlackPawn) bOpenFiles += 1;
    }

    score -= wOpenFiles * 15 * g_engineOptions.kingSafetyWeight / 100;
    score += bOpenFiles * 15 * g_engineOptions.kingSafetyWeight / 100;

    return score;
}

int evaluateRooks() {
    int score = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_ROOK) continue;

        int f = squareFile(i);
        int r = squareRank(i);
        int sign = (p.color == COLOR_WHITE) ? 1 : -1;

        bool isOpen = true, isSemiOpen = true;
        for (int rank = 0; rank < 8; rank++) {
            int sq = makeSquare(f, rank);
            const Piece& piece = g_board[sq];
            if (piece.piece == PIECE_PAWN) {
                if (piece.color == p.color) {
                    isOpen = false;
                    isSemiOpen = false;
                } else {
                    isOpen = false;
                }
            }
        }

        if (isOpen) score += g_engineOptions.rookOnOpenFile * sign;
        else if (isSemiOpen) score += g_engineOptions.rookOnSemiOpenFile * sign;

        if ((p.color == COLOR_WHITE && r == 6) || (p.color == COLOR_BLACK && r == 1)) {
            score += g_engineOptions.rookOnSeventhRank * sign;
        }

        if (r >= 4 && p.color == COLOR_WHITE) score += 2 * sign;
        if (r <= 3 && p.color == COLOR_BLACK) score -= 2 * sign;

        bool hasOwnRook = false;
        for (int file = 0; file < 8; file++) {
            int sq = makeSquare(file, r);
            if (sq != i && g_board[sq].piece == PIECE_ROOK && g_board[sq].color == p.color) {
                hasOwnRook = true;
                break;
            }
        }
        if (hasOwnRook) score += g_engineOptions.rookCoordinationBonus * sign;

        int kingSq = findKingSquare(p.color);
        int kingR = squareRank(kingSq);
        if (std::abs(r - kingR) <= 1) score += 3 * sign;
    }

    return score;
}

int evaluateKnights() {
    int score = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_KNIGHT) continue;

        int f = squareFile(i);
        int r = squareRank(i);
        int sign = (p.color == COLOR_WHITE) ? 1 : -1;

        if (f == 0 || f == 7 || r == 0 || r == 7) {
            score -= g_engineOptions.knightOnRimPenalty * sign;
        }

        bool isOutpost = false;
        if (p.color == COLOR_WHITE && r >= 3 && r <= 5) {
            bool protectedByPawn = false;
            if (isValidSquare(i - 17) && g_board[i - 17].piece == PIECE_PAWN && g_board[i - 17].color == COLOR_WHITE) protectedByPawn = true;
            if (isValidSquare(i - 15) && g_board[i - 15].piece == PIECE_PAWN && g_board[i - 15].color == COLOR_WHITE) protectedByPawn = true;

            bool canBeAttacked = false;
            for (int er = r + 1; er < 8; er++) {
                if (f > 0) {
                    int checkSq = makeSquare(f - 1, er);
                    if (g_board[checkSq].piece == PIECE_PAWN && g_board[checkSq].color == COLOR_BLACK) canBeAttacked = true;
                }
                if (f < 7) {
                    int checkSq = makeSquare(f + 1, er);
                    if (g_board[checkSq].piece == PIECE_PAWN && g_board[checkSq].color == COLOR_BLACK) canBeAttacked = true;
                }
            }

            if (protectedByPawn && !canBeAttacked) isOutpost = true;
        } else if (p.color == COLOR_BLACK && r >= 2 && r <= 4) {
            bool protectedByPawn = false;
            if (isValidSquare(i + 17) && g_board[i + 17].piece == PIECE_PAWN && g_board[i + 17].color == COLOR_BLACK) protectedByPawn = true;
            if (isValidSquare(i + 15) && g_board[i + 15].piece == PIECE_PAWN && g_board[i + 15].color == COLOR_BLACK) protectedByPawn = true;

            bool canBeAttacked = false;
            for (int er = r - 1; er >= 0; er--) {
                if (f > 0) {
                    int checkSq = makeSquare(f - 1, er);
                    if (g_board[checkSq].piece == PIECE_PAWN && g_board[checkSq].color == COLOR_WHITE) canBeAttacked = true;
                }
                if (f < 7) {
                    int checkSq = makeSquare(f + 1, er);
                    if (g_board[checkSq].piece == PIECE_PAWN && g_board[checkSq].color == COLOR_WHITE) canBeAttacked = true;
                }
            }

            if (protectedByPawn && !canBeAttacked) isOutpost = true;
        }

        if (isOutpost) score += g_engineOptions.knightOutpostBonus * sign;

        bool protectedByPawn = false;
        if (p.color == COLOR_WHITE) {
            if (isValidSquare(i - 17) && g_board[i - 17].piece == PIECE_PAWN && g_board[i - 17].color == COLOR_WHITE) protectedByPawn = true;
            if (isValidSquare(i - 15) && g_board[i - 15].piece == PIECE_PAWN && g_board[i - 15].color == COLOR_WHITE) protectedByPawn = true;
        } else {
            if (isValidSquare(i + 17) && g_board[i + 17].piece == PIECE_PAWN && g_board[i + 17].color == COLOR_BLACK) protectedByPawn = true;
            if (isValidSquare(i + 15) && g_board[i + 15].piece == PIECE_PAWN && g_board[i + 15].color == COLOR_BLACK) protectedByPawn = true;
        }
        if (protectedByPawn) score += 3 * sign;

        if (f >= 2 && f <= 5 && r >= 2 && r <= 5) score += 5 * sign;
    }

    return score;
}

int evaluateBishops() {
    int score = 0;
    int wBishops = 0, bBishops = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_BISHOP) continue;
        if (p.color == COLOR_WHITE) wBishops++;
        else bBishops++;
    }

    if (wBishops >= 2) score += g_engineOptions.bishopPairBonus;
    if (bBishops >= 2) score -= g_engineOptions.bishopPairBonus;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_BISHOP) continue;
        int sign = (p.color == COLOR_WHITE) ? 1 : -1;

        bool isBadBishop = true;
        int centerColor = (squareFile(i) + squareRank(i)) % 2;
        for (int j = 0; j < 128; j++) {
            if (!isValidSquare(j)) continue;
            const Piece& pawn = g_board[j];
            if (pawn.piece == PIECE_PAWN && pawn.color == p.color) {
                if ((squareFile(j) + squareRank(j)) % 2 == centerColor) {
                    isBadBishop = false;
                    break;
                }
            }
        }
        if (isBadBishop && wBishops + bBishops <= 3) {
            score -= g_engineOptions.badBishopPenalty * sign;
        }

        int mobility = 0;
        for (int d = 0; d < 4; d++) {
            int to = i + BISHOP_DELTAS[d];
            while (isValidSquare(to)) {
                if (g_board[to].piece == PIECE_NONE || g_board[to].color != p.color) mobility++;
                if (g_board[to].piece != PIECE_NONE) break;
                to += BISHOP_DELTAS[d];
            }
        }
        if (mobility <= 3) score -= static_cast<int>(g_engineOptions.trappedPiecePenalty * 0.3 * sign);

        bool isBehindPawn = false;
        if (p.color == COLOR_WHITE) {
            int checkSq = i + 16;
            if (isValidSquare(checkSq) && g_board[checkSq].piece == PIECE_PAWN && g_board[checkSq].color == COLOR_WHITE) isBehindPawn = true;
        } else {
            int checkSq = i - 16;
            if (isValidSquare(checkSq) && g_board[checkSq].piece == PIECE_PAWN && g_board[checkSq].color == COLOR_BLACK) isBehindPawn = true;
        }
        if (isBehindPawn) score += g_engineOptions.minorBehindPawnBonus * sign;

        if (squareFile(i) >= 2 && squareFile(i) <= 5 && squareRank(i) >= 2 && squareRank(i) <= 5) {
            score += 5 * sign;
        }

        int xrayCount = 0;
        for (int d = 0; d < 4; d++) {
            int to = i + BISHOP_DELTAS[d];
            int ownPieces = 0, enemyPieces = 0;
            while (isValidSquare(to)) {
                if (g_board[to].piece != PIECE_NONE) {
                    if (g_board[to].color == p.color) ownPieces++;
                    else enemyPieces++;
                    if (enemyPieces > 0 && ownPieces > 0) { xrayCount++; break; }
                    if (enemyPieces > 1) break;
                }
                to += BISHOP_DELTAS[d];
            }
        }
        if (xrayCount > 0) score += static_cast<int>(g_engineOptions.discoveryThreatBonus * 0.2 * sign * xrayCount);
    }

    return score;
}

int evaluateThreats() {
    int score = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE) continue;
        int sign = (p.color == COLOR_WHITE) ? 1 : -1;

        auto attackers = getAttackingPieces(i, 1 - p.color);
        auto defenders = getAttackingPieces(i, p.color);

        if (attackers.size() > defenders.size() && p.piece != PIECE_KING) {
            int penalty = HANGING_PIECE_PENALTY[p.piece];
            if (defenders.empty()) penalty = static_cast<int>(penalty * 1.5);
            score -= penalty * sign * g_engineOptions.threatWeight / 5;
        }

        if (p.piece == PIECE_QUEEN && g_gamePhase == GAME_PHASE_OPENING) {
            int r = squareRank(i);
            if (p.color == COLOR_WHITE && r >= 5) score -= g_engineOptions.queenEarlyDevelopmentPenalty * sign;
            if (p.color == COLOR_BLACK && r <= 2) score -= g_engineOptions.queenEarlyDevelopmentPenalty * sign;
        }

        if (p.piece == PIECE_KNIGHT || p.piece == PIECE_BISHOP) {
            bool trapped = true;
            int numDirs = (p.piece == PIECE_KNIGHT) ? 8 : 4;
            const int* deltas = (p.piece == PIECE_KNIGHT) ? KNIGHT_DELTAS : BISHOP_DELTAS;
            for (int j = 0; j < numDirs; j++) {
                int to = i + deltas[j];
                if (isValidSquare(to) && (g_board[to].piece == PIECE_NONE || g_board[to].color != p.color)) {
                    trapped = false;
                    break;
                }
            }
            if (trapped) score -= g_engineOptions.trappedPiecePenalty * sign;
        }
    }

    return score;
}

int evaluateSpace() {
    int score = 0;

    for (int r = 3; r <= 6; r++) {
        for (int f = 2; f <= 5; f++) {
            int sq = makeSquare(f, r);
            if (g_board[sq].piece != PIECE_NONE) {
                if (g_board[sq].color == COLOR_WHITE) score += 2;
                else score -= 2;
            }

            bool wControls = isSquareAttacked(sq, COLOR_WHITE);
            bool bControls = isSquareAttacked(sq, COLOR_BLACK);
            if (wControls && !bControls) score += 1;
            if (bControls && !wControls) score -= 1;
        }
    }

    for (int r = 2; r <= 5; r++) {
        for (int f = 1; f <= 6; f++) {
            int sq = makeSquare(f, r);
            if (g_board[sq].piece != PIECE_NONE) {
                if (g_board[sq].color == COLOR_WHITE) score += 1;
                else score -= 1;
            }
        }
    }

    return score * g_engineOptions.spaceWeight / 2;
}

int evaluateKingTropism() {
    int score = 0;
    int wKing = findKingSquare(COLOR_WHITE);
    int bKing = findKingSquare(COLOR_BLACK);
    if (wKing == -1 || bKing == -1) return 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE || p.piece == PIECE_PAWN || p.piece == PIECE_KING) continue;

        if (p.color == COLOR_WHITE) {
            int dist = squareDistance(i, bKing);
            score += (7 - dist) * getPieceValue(p.piece) / 1000 * g_engineOptions.kingTropismWeight;
        } else {
            int dist = squareDistance(i, wKing);
            score -= (7 - dist) * getPieceValue(p.piece) / 1000 * g_engineOptions.kingTropismWeight;
        }
    }

    return score;
}

int evaluatePinnedPieces() {
    int score = 0;

    for (int color = 0; color < 2; color++) {
        int kingSq = findKingSquare(color);
        if (kingSq == -1) continue;

        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i)) continue;
            const Piece& p = g_board[i];
            if (p.piece == PIECE_NONE || p.color != color) continue;
            if (p.piece == PIECE_KING) continue;

            bool isPinned = false;
            int kf = squareFile(kingSq), kr = squareRank(kingSq);
            int pf = squareFile(i), pr = squareRank(i);

            int df = (pf == kf) ? 0 : (pf > kf ? 1 : -1);
            int dr = (pr == kr) ? 0 : (pr > kr ? 1 : -1);
            if (std::abs(pf - kf) != std::abs(pr - kr) && pf != kf && pr != kr) continue;

            int f = pf + df, r = pr + dr;
            while (f >= 0 && f <= 7 && r >= 0 && r <= 7) {
                if (f == pf && r == pr) { f += df; r += dr; continue; }
                int sq = makeSquare(f, r);
                if (!isValidSquare(sq)) break;

                if (g_board[sq].piece != PIECE_NONE) {
                    if (g_board[sq].color != color) {
                        if ((dr != 0 && df != 0 && (g_board[sq].piece == PIECE_BISHOP || g_board[sq].piece == PIECE_QUEEN)) ||
                            (dr == 0 && df != 0 && (g_board[sq].piece == PIECE_ROOK || g_board[sq].piece == PIECE_QUEEN)) ||
                            (dr != 0 && df == 0 && (g_board[sq].piece == PIECE_ROOK || g_board[sq].piece == PIECE_QUEEN))) {
                            isPinned = true;
                        }
                    }
                    break;
                }
                f += df; r += dr;
            }

            if (isPinned) {
                if (color == COLOR_WHITE) score -= g_engineOptions.pinnedPiecePenalty * getPieceValue(p.piece) / 500;
                else score += g_engineOptions.pinnedPiecePenalty * getPieceValue(p.piece) / 500;
            }
        }
    }

    return score;
}

int evaluateMaterialImbalance() {
    int score = 0;
    int wKnights = 0, wBishops = 0, wRooks = 0, wQueens = 0;
    int bKnights = 0, bBishops = 0, bRooks = 0, bQueens = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE) continue;
        if (p.color == COLOR_WHITE) {
            if (p.piece == PIECE_KNIGHT) wKnights++;
            if (p.piece == PIECE_BISHOP) wBishops++;
            if (p.piece == PIECE_ROOK) wRooks++;
            if (p.piece == PIECE_QUEEN) wQueens++;
        } else {
            if (p.piece == PIECE_KNIGHT) bKnights++;
            if (p.piece == PIECE_BISHOP) bBishops++;
            if (p.piece == PIECE_ROOK) bRooks++;
            if (p.piece == PIECE_QUEEN) bQueens++;
        }
    }

    if (wKnights == 2 && wBishops == 0 && bKnights <= 1) score += 10;
    if (bKnights == 2 && bBishops == 0 && wKnights <= 1) score -= 10;

    if (wBishops >= 1 && wKnights >= 1) score += 5;
    if (bBishops >= 1 && bKnights >= 1) score -= 5;

    if (wRooks > bRooks && wQueens == bQueens) score += 10 * (wRooks - bRooks);
    if (bRooks > wRooks && wQueens == bQueens) score -= 10 * (bRooks - wRooks);

    int wMinor = wKnights + wBishops;
    int bMinor = bKnights + bBishops;
    if (wMinor > bMinor && wRooks < bRooks) score += 5 * (wMinor - bMinor);
    if (bMinor > wMinor && bRooks < wRooks) score -= 5 * (bMinor - wMinor);

    return score * g_engineOptions.materialImbalanceWeight / 5;
}

int evaluateInitiative() {
    int score = 0;
    int wMobility = 0, bMobility = 0;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE) continue;

        if (p.piece == PIECE_KNIGHT) {
            for (int j = 0; j < 8; j++) {
                int to = i + KNIGHT_DELTAS[j];
                if (isValidSquare(to) && (g_board[to].piece == PIECE_NONE || g_board[to].color != p.color)) {
                    if (p.color == COLOR_WHITE) wMobility++; else bMobility++;
                }
            }
        } else if (p.piece == PIECE_BISHOP) {
            for (int j = 0; j < 4; j++) {
                int to = i + BISHOP_DELTAS[j];
                while (isValidSquare(to)) {
                    if (g_board[to].piece == PIECE_NONE) {
                        if (p.color == COLOR_WHITE) wMobility++; else bMobility++;
                    } else {
                        if (g_board[to].color != p.color) {
                            if (p.color == COLOR_WHITE) wMobility++; else bMobility++;
                        }
                        break;
                    }
                    to += BISHOP_DELTAS[j];
                }
            }
        } else if (p.piece == PIECE_ROOK) {
            for (int j = 0; j < 4; j++) {
                int to = i + ROOK_DELTAS[j];
                while (isValidSquare(to)) {
                    if (g_board[to].piece == PIECE_NONE) {
                        if (p.color == COLOR_WHITE) wMobility++; else bMobility++;
                    } else {
                        if (g_board[to].color != p.color) {
                            if (p.color == COLOR_WHITE) wMobility++; else bMobility++;
                        }
                        break;
                    }
                    to += ROOK_DELTAS[j];
                }
            }
        }
    }

    if (wMobility > bMobility + 10) score += 5;
    if (bMobility > wMobility + 10) score -= 5;

    int wCenterControl = 0, bCenterControl = 0;
    for (int i = 0; i < 4; i++) {
        if (isSquareAttacked(CENTER_SQUARES[i], COLOR_WHITE)) wCenterControl++;
        if (isSquareAttacked(CENTER_SQUARES[i], COLOR_BLACK)) bCenterControl++;
    }
    if (wCenterControl > bCenterControl) score += 3;
    if (bCenterControl > wCenterControl) score -= 3;

    if (g_sideToMove == COLOR_WHITE) score += g_engineOptions.tempoBonus;
    else score -= g_engineOptions.tempoBonus;

    return score * g_engineOptions.initiativeWeight / 5;
}

int evaluateFull() {
    uint64_t hash = getPositionHash();
    auto it = g_evalCache.find(hash);
    if (it != g_evalCache.end()) {
        return it->second;
    }

    updateGamePhase();

    int score = 0;
    score += evaluateMaterial();
    score += evaluatePST();
    score += evaluateMobility();
    score += evaluatePawnStructure();
    score += evaluateKingSafety();
    score += evaluateRooks();
    score += evaluateKnights();
    score += evaluateBishops();
    score += evaluateThreats();
    score += evaluateSpace();
    score += evaluateKingTropism();
    score += evaluatePinnedPieces();
    score += evaluateMaterialImbalance();
    score += evaluateInitiative();

    if (g_engineOptions.contempt != 0 && g_gamePhase != GAME_PHASE_ENDGAME) {
        if (g_sideToMove == COLOR_WHITE) score += g_engineOptions.contempt;
        else score -= g_engineOptions.contempt;
    }

    if (g_sideToMove == COLOR_BLACK) score = -score;

    g_evalCache[hash] = score;
    return score;
}

}
