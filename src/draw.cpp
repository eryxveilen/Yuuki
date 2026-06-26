#include "draw.h"
#include "globals.h"
#include "utils.h"
#include "board.h"

namespace Yuuki {

bool isInsufficientMaterial() {
    std::vector<int> wPieces, bPieces;

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece == PIECE_NONE || p.piece == PIECE_KING) continue;
        if (p.color == COLOR_WHITE) wPieces.push_back(p.piece);
        else bPieces.push_back(p.piece);
    }

    if (wPieces.empty() && bPieces.empty()) return true;
    if (wPieces.empty() && bPieces.size() == 1 && (bPieces[0] == PIECE_KNIGHT || bPieces[0] == PIECE_BISHOP)) return true;
    if (bPieces.empty() && wPieces.size() == 1 && (wPieces[0] == PIECE_KNIGHT || wPieces[0] == PIECE_BISHOP)) return true;
    if (wPieces.size() == 1 && bPieces.size() == 1 && wPieces[0] == PIECE_BISHOP && bPieces[0] == PIECE_BISHOP) {
        int wSqColor = -1, bSqColor = -1;
        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i)) continue;
            const Piece& p = g_board[i];
            if (p.piece == PIECE_BISHOP) {
                int sqColor = (squareFile(i) + squareRank(i)) % 2;
                if (p.color == COLOR_WHITE) wSqColor = sqColor;
                else bSqColor = sqColor;
            }
        }
        if (wSqColor == bSqColor) return true;
    }
    if (wPieces.size() == 2 && bPieces.empty()) {
        if (wPieces[0] == PIECE_KNIGHT && wPieces[1] == PIECE_KNIGHT) return true;
    }
    if (bPieces.size() == 2 && wPieces.empty()) {
        if (bPieces[0] == PIECE_KNIGHT && bPieces[1] == PIECE_KNIGHT) return true;
    }

    return false;
}

bool isThreefoldRepetition() {
    uint64_t currentHash = getPositionHash();
    int count = 0;
    for (auto hash : g_positionHistory) {
        if (hash == currentHash) {
            count++;
            if (count >= 2) return true;
        }
    }
    return false;
}

int repetitionCount() {
    uint64_t currentHash = getPositionHash();
    int count = 0;
    for (auto hash : g_positionHistory) {
        if (hash == currentHash) count++;
    }
    return count;
}

bool isDraw() {
    if (g_halfMoveClock >= 100) return true;
    if (isInsufficientMaterial()) return true;
    if (isThreefoldRepetition()) return true;
    return false;
}

}
