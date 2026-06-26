#include "see.h"
#include "globals.h"
#include "utils.h"
#include <algorithm>
#include <vector>

namespace Yuuki {

int see(int sq) {
    int seeGain[32];
    Piece occupiers[128];
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) occupiers[i] = g_board[i];
        else occupiers[i] = {PIECE_NONE, 0};
    }

    int targetPiece = occupiers[sq].piece;
    int depth = 0;
    int currentColor = g_sideToMove;

    seeGain[depth] = targetPiece != PIECE_NONE ? SEE_PIECE_VALUES[targetPiece] : 0;

    do {
        std::vector<std::pair<int, int>> attackers;
        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i) || occupiers[i].piece == PIECE_NONE || occupiers[i].color != currentColor) continue;
            const Piece& piece = occupiers[i];
            if (piece.piece == PIECE_PAWN) {
                if (currentColor == COLOR_WHITE) {
                    if (i + 15 == sq || i + 17 == sq) {
                        attackers.push_back({SEE_PIECE_VALUES[piece.piece], i});
                    }
                } else {
                    if (i - 15 == sq || i - 17 == sq) {
                        attackers.push_back({SEE_PIECE_VALUES[piece.piece], i});
                    }
                }
            } else if (piece.piece == PIECE_KNIGHT) {
                for (int j = 0; j < 8; j++) {
                    if (i + KNIGHT_DELTAS[j] == sq) {
                        attackers.push_back({SEE_PIECE_VALUES[piece.piece], i});
                        break;
                    }
                }
            } else if (piece.piece == PIECE_BISHOP || piece.piece == PIECE_ROOK || piece.piece == PIECE_QUEEN) {
                const int* deltas = (piece.piece == PIECE_BISHOP) ? BISHOP_DELTAS : (piece.piece == PIECE_ROOK ? ROOK_DELTAS : QUEEN_DELTAS);
                int numDeltas = (piece.piece == PIECE_QUEEN) ? 8 : 4;
                for (int j = 0; j < numDeltas; j++) {
                    int to = i + deltas[j];
                    bool found = false;
                    while (isValidSquare(to)) {
                        if (to == sq) {
                            attackers.push_back({SEE_PIECE_VALUES[piece.piece], i});
                            found = true;
                            break;
                        }
                        if (occupiers[to].piece != PIECE_NONE) break;
                        to += deltas[j];
                    }
                    if (found) break;
                }
            } else if (piece.piece == PIECE_KING) {
                for (int j = 0; j < 8; j++) {
                    if (i + KING_DELTAS[j] == sq) {
                        attackers.push_back({SEE_PIECE_VALUES[piece.piece], i});
                        break;
                    }
                }
            }
        }

        if (attackers.empty()) break;

        std::sort(attackers.begin(), attackers.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        auto attacker = attackers[0];
        depth++;
        seeGain[depth] = SEE_PIECE_VALUES[targetPiece] - seeGain[depth - 1];
        targetPiece = occupiers[attacker.second].piece;
        occupiers[attacker.second] = {PIECE_NONE, 0};
        currentColor = 1 - currentColor;

    } while (depth < 31);

    while (depth > 0) {
        seeGain[depth - 1] = -std::max(-seeGain[depth - 1], seeGain[depth]);
        depth--;
    }

    return seeGain[0];
}

}
