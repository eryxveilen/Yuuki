#include "attack.h"
#include "globals.h"
#include "utils.h"

namespace Yuuki {

bool isSquareAttacked(int sq, int byColor) {
    if (byColor == COLOR_WHITE) {
        if (isValidSquare(sq - 15)) {
            const Piece& p = g_board[sq - 15];
            if (p.piece == PIECE_PAWN && p.color == COLOR_WHITE) return true;
        }
        if (isValidSquare(sq - 17)) {
            const Piece& p = g_board[sq - 17];
            if (p.piece == PIECE_PAWN && p.color == COLOR_WHITE) return true;
        }
    } else {
        if (isValidSquare(sq + 15)) {
            const Piece& p = g_board[sq + 15];
            if (p.piece == PIECE_PAWN && p.color == COLOR_BLACK) return true;
        }
        if (isValidSquare(sq + 17)) {
            const Piece& p = g_board[sq + 17];
            if (p.piece == PIECE_PAWN && p.color == COLOR_BLACK) return true;
        }
    }

    for (int i = 0; i < 8; i++) {
        int from = sq + KNIGHT_DELTAS[i];
        if (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece == PIECE_KNIGHT && p.color == byColor) return true;
        }
    }

    for (int i = 0; i < 4; i++) {
        int dir = BISHOP_DELTAS[i];
        int from = sq + dir;
        while (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece != PIECE_NONE) {
                if (p.color == byColor && (p.piece == PIECE_BISHOP || p.piece == PIECE_QUEEN)) return true;
                break;
            }
            from += dir;
        }
    }

    for (int i = 0; i < 4; i++) {
        int dir = ROOK_DELTAS[i];
        int from = sq + dir;
        while (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece != PIECE_NONE) {
                if (p.color == byColor && (p.piece == PIECE_ROOK || p.piece == PIECE_QUEEN)) return true;
                break;
            }
            from += dir;
        }
    }

    for (int i = 0; i < 8; i++) {
        int from = sq + KING_DELTAS[i];
        if (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece == PIECE_KING && p.color == byColor) return true;
        }
    }

    return false;
}

int findKingSquare(int color) {
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) {
            const Piece& p = g_board[i];
            if (p.piece == PIECE_KING && p.color == color) {
                return i;
            }
        }
    }
    return -1;
}

bool isInCheck(int color) {
    int kingSq = findKingSquare(color);
    if (kingSq == -1) return false;
    return isSquareAttacked(kingSq, 1 - color);
}

bool pieceAttacksSquare(int fromSq, const Piece& piece, int toSq) {
    if (piece.piece == PIECE_PAWN) {
        if (piece.color == COLOR_WHITE) {
            return (fromSq + 15 == toSq) || (fromSq + 17 == toSq);
        } else {
            return (fromSq - 15 == toSq) || (fromSq - 17 == toSq);
        }
    }
    if (piece.piece == PIECE_KNIGHT) {
        for (int i = 0; i < 8; i++) {
            if (fromSq + KNIGHT_DELTAS[i] == toSq) return true;
        }
        return false;
    }
    if (piece.piece == PIECE_KING) {
        for (int i = 0; i < 8; i++) {
            if (fromSq + KING_DELTAS[i] == toSq) return true;
        }
        return false;
    }
    if (piece.piece == PIECE_BISHOP || piece.piece == PIECE_ROOK || piece.piece == PIECE_QUEEN) {
        const int* deltas = (piece.piece == PIECE_BISHOP) ? BISHOP_DELTAS : (piece.piece == PIECE_ROOK ? ROOK_DELTAS : QUEEN_DELTAS);
        int numDeltas = (piece.piece == PIECE_QUEEN) ? 8 : 4;
        int fromFile = squareFile(fromSq);
        int fromRank = squareRank(fromSq);
        int toFile = squareFile(toSq);
        int toRank = squareRank(toSq);

        for (int i = 0; i < numDeltas; i++) {
            int dir = deltas[i];
            int df = squareFile(fromSq + dir) - fromFile;
            int dr = squareRank(fromSq + dir) - fromRank;
            if (df == 0 && dr == 0) continue;

            int steps = 0;
            if (df != 0) steps = (toFile - fromFile) / df;
            else if (dr != 0) steps = (toRank - fromRank) / dr;

            if (steps <= 0) continue;

            bool valid = true;
            int current = fromSq;
            for (int s = 0; s < steps; s++) {
                current += dir;
                if (!isValidSquare(current)) {
                    valid = false;
                    break;
                }
                if (s < steps - 1 && g_board[current].piece != PIECE_NONE) {
                    valid = false;
                    break;
                }
            }
            if (valid && current == toSq) return true;
        }
        return false;
    }
    return false;
}

bool pieceCanMoveTo(int fromSq, const Piece& piece, int toSq) {
    return pieceAttacksSquare(fromSq, piece, toSq);
}

std::vector<AttackerInfo> getAttackingPieces(int sq, int byColor) {
    std::vector<AttackerInfo> attackers;
    if (byColor == COLOR_WHITE) {
        if (isValidSquare(sq - 15)) {
            const Piece& p = g_board[sq - 15];
            if (p.piece == PIECE_PAWN && p.color == COLOR_WHITE) attackers.push_back({sq - 15, PIECE_PAWN});
        }
        if (isValidSquare(sq - 17)) {
            const Piece& p = g_board[sq - 17];
            if (p.piece == PIECE_PAWN && p.color == COLOR_WHITE) attackers.push_back({sq - 17, PIECE_PAWN});
        }
    } else {
        if (isValidSquare(sq + 15)) {
            const Piece& p = g_board[sq + 15];
            if (p.piece == PIECE_PAWN && p.color == COLOR_BLACK) attackers.push_back({sq + 15, PIECE_PAWN});
        }
        if (isValidSquare(sq + 17)) {
            const Piece& p = g_board[sq + 17];
            if (p.piece == PIECE_PAWN && p.color == COLOR_BLACK) attackers.push_back({sq + 17, PIECE_PAWN});
        }
    }
    for (int i = 0; i < 8; i++) {
        int from = sq + KNIGHT_DELTAS[i];
        if (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece == PIECE_KNIGHT && p.color == byColor) attackers.push_back({from, PIECE_KNIGHT});
        }
    }
    for (int i = 0; i < 4; i++) {
        int dir = BISHOP_DELTAS[i];
        int from = sq + dir;
        while (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece != PIECE_NONE) {
                if (p.color == byColor && (p.piece == PIECE_BISHOP || p.piece == PIECE_QUEEN))
                    attackers.push_back({from, p.piece});
                break;
            }
            from += dir;
        }
    }
    for (int i = 0; i < 4; i++) {
        int dir = ROOK_DELTAS[i];
        int from = sq + dir;
        while (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece != PIECE_NONE) {
                if (p.color == byColor && (p.piece == PIECE_ROOK || p.piece == PIECE_QUEEN))
                    attackers.push_back({from, p.piece});
                break;
            }
            from += dir;
        }
    }
    for (int i = 0; i < 8; i++) {
        int from = sq + KING_DELTAS[i];
        if (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece == PIECE_KING && p.color == byColor) attackers.push_back({from, PIECE_KING});
        }
    }
    return attackers;
}

int countAttackers(int sq, int byColor) {
    int count = 0;
    if (byColor == COLOR_WHITE) {
        if (isValidSquare(sq - 15)) {
            const Piece& p = g_board[sq - 15];
            if (p.piece == PIECE_PAWN && p.color == COLOR_WHITE) count++;
        }
        if (isValidSquare(sq - 17)) {
            const Piece& p = g_board[sq - 17];
            if (p.piece == PIECE_PAWN && p.color == COLOR_WHITE) count++;
        }
    } else {
        if (isValidSquare(sq + 15)) {
            const Piece& p = g_board[sq + 15];
            if (p.piece == PIECE_PAWN && p.color == COLOR_BLACK) count++;
        }
        if (isValidSquare(sq + 17)) {
            const Piece& p = g_board[sq + 17];
            if (p.piece == PIECE_PAWN && p.color == COLOR_BLACK) count++;
        }
    }
    for (int i = 0; i < 8; i++) {
        int from = sq + KNIGHT_DELTAS[i];
        if (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece == PIECE_KNIGHT && p.color == byColor) count++;
        }
    }
    for (int i = 0; i < 4; i++) {
        int dir = BISHOP_DELTAS[i];
        int from = sq + dir;
        while (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece != PIECE_NONE) {
                if (p.color == byColor && (p.piece == PIECE_BISHOP || p.piece == PIECE_QUEEN)) count++;
                break;
            }
            from += dir;
        }
    }
    for (int i = 0; i < 4; i++) {
        int dir = ROOK_DELTAS[i];
        int from = sq + dir;
        while (isValidSquare(from)) {
            const Piece& p = g_board[from];
            if (p.piece != PIECE_NONE) {
                if (p.color == byColor && (p.piece == PIECE_ROOK || p.piece == PIECE_QUEEN)) count++;
                break;
            }
            from += dir;
        }
    }
    return count;
}

}
