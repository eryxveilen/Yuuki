#include "move.h"
#include "globals.h"
#include "utils.h"
#include "attack.h"
#include <sstream>
#include <cctype>

namespace Yuuki {

void makeMove(const Move& move) {
    UndoInfo undoInfo;
    undoInfo.move = move;
    undoInfo.castlingRights = g_castlingRights;
    undoInfo.enPassantSquare = g_enPassantSquare;
    undoInfo.halfMoveClock = g_halfMoveClock;
    undoInfo.capturedPiece = g_board[move.to].piece;
    undoInfo.capturedColor = g_board[move.to].color;
    undoInfo.sideToMove = g_sideToMove;
    undoInfo.isNull = false;
    undoInfo.epCaptureSq = -1;
    undoInfo.castleRookFrom = -1;
    undoInfo.castleRookTo = -1;

    g_board[move.to] = {move.piece, static_cast<uint8_t>(g_sideToMove)};
    g_board[move.from] = {PIECE_NONE, 0};

    if (move.promotion != PIECE_NONE) {
        g_board[move.to] = {move.promotion, static_cast<uint8_t>(g_sideToMove)};
    }

    if (move.flags & FLAG_ENPASSANT) {
        int capturedPawnSq = move.to + (g_sideToMove == COLOR_WHITE ? -16 : 16);
        undoInfo.epCaptureSq = capturedPawnSq;
        undoInfo.epCapturePiece = g_board[capturedPawnSq].piece;
        undoInfo.epCaptureColor = g_board[capturedPawnSq].color;
        g_board[capturedPawnSq] = {PIECE_NONE, 0};
    }

    if (move.flags & FLAG_CASTLING) {
        if (move.to == SQUARE_G1) {
            undoInfo.castleRookFrom = SQUARE_H1;
            undoInfo.castleRookTo = SQUARE_F1;
            g_board[SQUARE_F1] = g_board[SQUARE_H1];
            g_board[SQUARE_H1] = {PIECE_NONE, 0};
        } else if (move.to == SQUARE_C1) {
            undoInfo.castleRookFrom = SQUARE_A1;
            undoInfo.castleRookTo = SQUARE_D1;
            g_board[SQUARE_D1] = g_board[SQUARE_A1];
            g_board[SQUARE_A1] = {PIECE_NONE, 0};
        } else if (move.to == SQUARE_G8) {
            undoInfo.castleRookFrom = SQUARE_H8;
            undoInfo.castleRookTo = SQUARE_F8;
            g_board[SQUARE_F8] = g_board[SQUARE_H8];
            g_board[SQUARE_H8] = {PIECE_NONE, 0};
        } else if (move.to == SQUARE_C8) {
            undoInfo.castleRookFrom = SQUARE_A8;
            undoInfo.castleRookTo = SQUARE_D8;
            g_board[SQUARE_D8] = g_board[SQUARE_A8];
            g_board[SQUARE_A8] = {PIECE_NONE, 0};
        }
    }

    if (move.piece == PIECE_KING) {
        if (g_sideToMove == COLOR_WHITE) {
            g_castlingRights &= ~(CASTLE_WK | CASTLE_WQ);
        } else {
            g_castlingRights &= ~(CASTLE_BK | CASTLE_BQ);
        }
    }

    if (move.from == SQUARE_A1 || move.to == SQUARE_A1) g_castlingRights &= ~CASTLE_WQ;
    if (move.from == SQUARE_H1 || move.to == SQUARE_H1) g_castlingRights &= ~CASTLE_WK;
    if (move.from == SQUARE_A8 || move.to == SQUARE_A8) g_castlingRights &= ~CASTLE_BQ;
    if (move.from == SQUARE_H8 || move.to == SQUARE_H8) g_castlingRights &= ~CASTLE_BK;

    if (move.flags & FLAG_PAWN_DOUBLE) {
        g_enPassantSquare = move.from + (g_sideToMove == COLOR_WHITE ? 16 : -16);
    } else {
        g_enPassantSquare = -1;
    }

    if (move.piece == PIECE_PAWN || move.captured != PIECE_NONE) {
        g_halfMoveClock = 0;
    } else {
        g_halfMoveClock++;
    }

    if (g_sideToMove == COLOR_BLACK) {
        g_fullMoveNumber++;
    }

    g_sideToMove = 1 - g_sideToMove;
    g_moveHistory.push_back(undoInfo);
    updateGamePhase();
}

void undoMove() {
    if (g_moveHistory.empty()) return;

    UndoInfo undoInfo = g_moveHistory.back();
    g_moveHistory.pop_back();
    const Move& move = undoInfo.move;

    g_sideToMove = undoInfo.sideToMove;

    g_board[move.from] = {move.piece, static_cast<uint8_t>(g_sideToMove)};

    if (move.flags & FLAG_ENPASSANT) {
        g_board[move.to] = {PIECE_NONE, 0};
        if (undoInfo.epCaptureSq != -1) {
            g_board[undoInfo.epCaptureSq] = {undoInfo.epCapturePiece, undoInfo.epCaptureColor};
        }
    } else {
        g_board[move.to] = {undoInfo.capturedPiece, undoInfo.capturedColor};
    }

    if (move.flags & FLAG_CASTLING && undoInfo.castleRookFrom != -1) {
        g_board[undoInfo.castleRookFrom] = g_board[undoInfo.castleRookTo];
        g_board[undoInfo.castleRookTo] = {PIECE_NONE, 0};
    }

    g_castlingRights = undoInfo.castlingRights;
    g_enPassantSquare = undoInfo.enPassantSquare;
    g_halfMoveClock = undoInfo.halfMoveClock;

    if (g_sideToMove == COLOR_BLACK) {
        g_fullMoveNumber--;
    }

    updateGamePhase();
}

void makeNullMove() {
    UndoInfo undoInfo;
    undoInfo.isNull = true;
    undoInfo.sideToMove = g_sideToMove;
    undoInfo.enPassantSquare = g_enPassantSquare;
    undoInfo.halfMoveClock = g_halfMoveClock;

    g_enPassantSquare = -1;
    g_sideToMove = 1 - g_sideToMove;
    if (g_sideToMove == COLOR_BLACK) {
        g_fullMoveNumber++;
    }
    g_moveHistory.push_back(undoInfo);
}

void undoNullMove() {
    if (g_moveHistory.empty()) return;
    UndoInfo info = g_moveHistory.back();
    g_moveHistory.pop_back();
    if (!info.isNull) {
        g_moveHistory.push_back(info);
        return;
    }
    if (g_sideToMove == COLOR_WHITE) {
        g_fullMoveNumber--;
    }
    g_sideToMove = info.sideToMove;
    g_enPassantSquare = info.enPassantSquare;
    g_halfMoveClock = info.halfMoveClock;
}

void generatePawnMoves(int from, int color, std::vector<Move>& moves, bool capturesOnly) {
    int direction = (color == COLOR_WHITE) ? 16 : -16;
    int startRank = (color == COLOR_WHITE) ? 1 : 6;
    int promoRank = (color == COLOR_WHITE) ? 7 : 0;
    int to = from + direction;

    if (!capturesOnly && isValidSquare(to) && g_board[to].piece == PIECE_NONE) {
        int r = squareRank(to);
        if (r == promoRank) {
            moves.push_back(createMove(from, to, PIECE_PAWN, PIECE_NONE, PIECE_QUEEN, FLAG_PROMOTION));
            moves.push_back(createMove(from, to, PIECE_PAWN, PIECE_NONE, PIECE_ROOK, FLAG_PROMOTION));
            moves.push_back(createMove(from, to, PIECE_PAWN, PIECE_NONE, PIECE_BISHOP, FLAG_PROMOTION));
            moves.push_back(createMove(from, to, PIECE_PAWN, PIECE_NONE, PIECE_KNIGHT, FLAG_PROMOTION));
        } else {
            moves.push_back(createMove(from, to, PIECE_PAWN, PIECE_NONE, PIECE_NONE, FLAG_NONE));
        }

        int fromRank = squareRank(from);
        if (fromRank == startRank) {
            int doubleTo = from + direction * 2;
            if (isValidSquare(doubleTo) && g_board[doubleTo].piece == PIECE_NONE) {
                moves.push_back(createMove(from, doubleTo, PIECE_PAWN, PIECE_NONE, PIECE_NONE, FLAG_PAWN_DOUBLE));
            }
        }
    }

    for (int i = -1; i <= 1; i += 2) {
        int captureTo = to + i;
        if (!isValidSquare(captureTo)) continue;

        const Piece& target = g_board[captureTo];
        if (target.piece != PIECE_NONE && target.color != color) {
            int cr = squareRank(captureTo);
            if (cr == promoRank) {
                moves.push_back(createMove(from, captureTo, PIECE_PAWN, target.piece, PIECE_QUEEN, FLAG_PROMOTION));
                moves.push_back(createMove(from, captureTo, PIECE_PAWN, target.piece, PIECE_ROOK, FLAG_PROMOTION));
                moves.push_back(createMove(from, captureTo, PIECE_PAWN, target.piece, PIECE_BISHOP, FLAG_PROMOTION));
                moves.push_back(createMove(from, captureTo, PIECE_PAWN, target.piece, PIECE_KNIGHT, FLAG_PROMOTION));
            } else {
                moves.push_back(createMove(from, captureTo, PIECE_PAWN, target.piece, PIECE_NONE, FLAG_NONE));
            }
        }

        if (captureTo == g_enPassantSquare) {
            moves.push_back(createMove(from, captureTo, PIECE_PAWN, PIECE_PAWN, PIECE_NONE, FLAG_ENPASSANT));
        }
    }
}

void generateKnightMoves(int from, int color, std::vector<Move>& moves, bool capturesOnly) {
    const Piece& piece = g_board[from];
    for (int i = 0; i < 8; i++) {
        int to = from + KNIGHT_DELTAS[i];
        if (!isValidSquare(to)) continue;
        const Piece& target = g_board[to];
        if (target.piece == PIECE_NONE) {
            if (!capturesOnly) {
                moves.push_back(createMove(from, to, piece.piece, PIECE_NONE, PIECE_NONE, FLAG_NONE));
            }
        } else if (target.color != color) {
            moves.push_back(createMove(from, to, piece.piece, target.piece, PIECE_NONE, FLAG_NONE));
        }
    }
}

void generateSlidingMoves(int from, const int* deltas, int numDeltas, int color, std::vector<Move>& moves, bool capturesOnly) {
    const Piece& piece = g_board[from];
    for (int i = 0; i < numDeltas; i++) {
        int dir = deltas[i];
        int to = from + dir;
        while (isValidSquare(to)) {
            const Piece& target = g_board[to];
            if (target.piece == PIECE_NONE) {
                if (!capturesOnly) {
                    moves.push_back(createMove(from, to, piece.piece, PIECE_NONE, PIECE_NONE, FLAG_NONE));
                }
            } else {
                if (target.color != color) {
                    moves.push_back(createMove(from, to, piece.piece, target.piece, PIECE_NONE, FLAG_NONE));
                }
                break;
            }
            to += dir;
        }
    }
}

void generateKingMoves(int from, int color, std::vector<Move>& moves, bool capturesOnly) {
    for (int i = 0; i < 8; i++) {
        int to = from + KING_DELTAS[i];
        if (!isValidSquare(to)) continue;
        const Piece& target = g_board[to];
        if (target.piece == PIECE_NONE) {
            if (!capturesOnly) {
                moves.push_back(createMove(from, to, PIECE_KING, PIECE_NONE, PIECE_NONE, FLAG_NONE));
            }
        } else if (target.color != color) {
            moves.push_back(createMove(from, to, PIECE_KING, target.piece, PIECE_NONE, FLAG_NONE));
        }
    }

    if (!capturesOnly && !isInCheck(color)) {
        if (color == COLOR_WHITE) {
            if ((g_castlingRights & CASTLE_WK) &&
                g_board[SQUARE_F1].piece == PIECE_NONE &&
                g_board[SQUARE_G1].piece == PIECE_NONE &&
                !isSquareAttacked(SQUARE_F1, COLOR_BLACK) &&
                !isSquareAttacked(SQUARE_G1, COLOR_BLACK)) {
                moves.push_back(createMove(SQUARE_E1, SQUARE_G1, PIECE_KING, PIECE_NONE, PIECE_NONE, FLAG_CASTLING));
            }
            if ((g_castlingRights & CASTLE_WQ) &&
                g_board[SQUARE_D1].piece == PIECE_NONE &&
                g_board[SQUARE_C1].piece == PIECE_NONE &&
                g_board[SQUARE_B1].piece == PIECE_NONE &&
                !isSquareAttacked(SQUARE_D1, COLOR_BLACK) &&
                !isSquareAttacked(SQUARE_C1, COLOR_BLACK)) {
                moves.push_back(createMove(SQUARE_E1, SQUARE_C1, PIECE_KING, PIECE_NONE, PIECE_NONE, FLAG_CASTLING));
            }
        } else {
            if ((g_castlingRights & CASTLE_BK) &&
                g_board[SQUARE_F8].piece == PIECE_NONE &&
                g_board[SQUARE_G8].piece == PIECE_NONE &&
                !isSquareAttacked(SQUARE_F8, COLOR_WHITE) &&
                !isSquareAttacked(SQUARE_G8, COLOR_WHITE)) {
                moves.push_back(createMove(SQUARE_E8, SQUARE_G8, PIECE_KING, PIECE_NONE, PIECE_NONE, FLAG_CASTLING));
            }
            if ((g_castlingRights & CASTLE_BQ) &&
                g_board[SQUARE_D8].piece == PIECE_NONE &&
                g_board[SQUARE_C8].piece == PIECE_NONE &&
                g_board[SQUARE_B8].piece == PIECE_NONE &&
                !isSquareAttacked(SQUARE_D8, COLOR_WHITE) &&
                !isSquareAttacked(SQUARE_C8, COLOR_WHITE)) {
                moves.push_back(createMove(SQUARE_E8, SQUARE_C8, PIECE_KING, PIECE_NONE, PIECE_NONE, FLAG_CASTLING));
            }
        }
    }
}

std::vector<Move> generatePseudoLegalMoves(int color, bool capturesOnly) {
    std::vector<Move> moves;
    moves.reserve(MAX_MOVES);

    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& piece = g_board[i];
        if (piece.piece == PIECE_NONE || piece.color != color) continue;

        switch (piece.piece) {
            case PIECE_PAWN:
                generatePawnMoves(i, color, moves, capturesOnly);
                break;
            case PIECE_KNIGHT:
                generateKnightMoves(i, color, moves, capturesOnly);
                break;
            case PIECE_BISHOP:
                generateSlidingMoves(i, BISHOP_DELTAS, 4, color, moves, capturesOnly);
                break;
            case PIECE_ROOK:
                generateSlidingMoves(i, ROOK_DELTAS, 4, color, moves, capturesOnly);
                break;
            case PIECE_QUEEN:
                generateSlidingMoves(i, QUEEN_DELTAS, 8, color, moves, capturesOnly);
                break;
            case PIECE_KING:
                generateKingMoves(i, color, moves, capturesOnly);
                break;
        }
    }

    return moves;
}

std::vector<Move> generateLegalMovesImpl(int color) {
    auto pseudoMoves = generatePseudoLegalMoves(color, false);
    std::vector<Move> legalMoves;
    legalMoves.reserve(pseudoMoves.size());

    for (const auto& move : pseudoMoves) {
        makeMove(move);
        if (!isInCheck(color)) {
            legalMoves.push_back(move);
        }
        undoMove();
    }

    return legalMoves;
}

std::vector<Move> generateCaptureMoves(int color) {
    return generatePseudoLegalMoves(color, true);
}

std::vector<Move> generateEvasions(int color) {
    int kingSq = findKingSquare(color);
    std::vector<Move> moves;

    for (int i = 0; i < 8; i++) {
        int to = kingSq + KING_DELTAS[i];
        if (!isValidSquare(to)) continue;
        if (!isSquareAttacked(to, 1 - color)) {
            const Piece& target = g_board[to];
            if (target.piece == PIECE_NONE) {
                moves.push_back(createMove(kingSq, to, PIECE_KING, PIECE_NONE, PIECE_NONE, FLAG_NONE));
            } else if (target.color != color) {
                moves.push_back(createMove(kingSq, to, PIECE_KING, target.piece, PIECE_NONE, FLAG_NONE));
            }
        }
    }

    std::vector<int> checkers;
    for (int i = 0; i < 128; i++) {
        if (!isValidSquare(i)) continue;
        const Piece& p = g_board[i];
        if (p.piece != PIECE_NONE && p.color != color) {
            if (pieceAttacksSquare(i, p, kingSq)) {
                checkers.push_back(i);
            }
        }
    }

    if (checkers.size() == 1) {
        int checkerSq = checkers[0];
        const Piece& checkerPiece = g_board[checkerSq];

        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i)) continue;
            const Piece& piece = g_board[i];
            if (piece.piece == PIECE_NONE || piece.color != color || piece.piece == PIECE_KING) continue;

            auto between = getSquaresBetween(checkerSq, kingSq);
            for (size_t j = 0; j < between.size(); j++) {
                if (pieceCanMoveTo(i, piece, between[j])) {
                    moves.push_back(createMove(i, between[j], piece.piece, PIECE_NONE, PIECE_NONE, FLAG_NONE));
                }
            }

            if (pieceCanMoveTo(i, piece, checkerSq)) {
                moves.push_back(createMove(i, checkerSq, piece.piece, checkerPiece.piece, PIECE_NONE, FLAG_NONE));
            }
        }
    }

    return moves;
}

std::vector<int> getSquaresBetween(int sq1, int sq2) {
    std::vector<int> result;
    int f1 = squareFile(sq1), r1 = squareRank(sq1);
    int f2 = squareFile(sq2), r2 = squareRank(sq2);

    if (f1 != f2 && r1 != r2 && std::abs(f1 - f2) != std::abs(r1 - r2)) return result;

    int df = (f2 > f1) ? 1 : (f2 < f1 ? -1 : 0);
    int dr = (r2 > r1) ? 1 : (r2 < r1 ? -1 : 0);

    int f = f1 + df;
    int r = r1 + dr;

    while (f != f2 || r != r2) {
        result.push_back(makeSquare(f, r));
        f += df;
        r += dr;
    }

    return result;
}

std::string moveToString(const Move& move) {
    if (move.from == 0 && move.to == 0) return "0000";
    char fromFile = 'a' + (move.from & 7);
    char fromRank = '1' + (move.from >> 4);
    char toFile = 'a' + (move.to & 7);
    char toRank = '1' + (move.to >> 4);
    std::string str;
    str += fromFile; str += fromRank; str += toFile; str += toRank;
    if (move.promotion != PIECE_NONE) {
        str += g_pieceToChar[move.promotion];
    }
    return str;
}

std::string moveToSAN(const Move& move) {
    if (move.flags & FLAG_CASTLING) {
        return (move.to > move.from) ? "O-O" : "O-O-O";
    }

    std::string pieceChars = " PNBRQK";
    std::string san;
    if (move.piece != PIECE_PAWN) san += pieceChars[move.piece];

    char fromFile = 'a' + (move.from & 7);
    char fromRank = '1' + (move.from >> 4);
    char toFile = 'a' + (move.to & 7);
    char toRank = '1' + (move.to >> 4);

    if (move.piece == PIECE_PAWN) {
        if (move.captured != PIECE_NONE || (move.flags & FLAG_ENPASSANT)) {
            san += fromFile;
            san += 'x';
        }
        san += toFile;
        san += toRank;
        if (move.promotion != PIECE_NONE) {
            san += "=";
            san += std::tolower(pieceChars[move.promotion]);
        }
    } else {
        bool ambiguous = false;
        bool sameFile = false;
        bool sameRank = false;

        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i)) continue;
            const Piece& p = g_board[i];
            if (p.piece != move.piece || p.color != g_sideToMove || i == move.from) continue;

            makeMove(createMove(i, move.to, move.piece, g_board[move.to].piece, move.promotion, move.flags));
            bool wasLegal = !isInCheck(g_sideToMove);
            undoMove();

            if (wasLegal) {
                ambiguous = true;
                if (squareFile(i) == squareFile(move.from)) sameFile = true;
                if (squareRank(i) == squareRank(move.from)) sameRank = true;
            }
        }

        if (ambiguous) {
            if (!sameFile) san += fromFile;
            else if (!sameRank) san += fromRank;
            else { san += fromFile; san += fromRank; }
        }

        if (move.captured != PIECE_NONE) san += 'x';
        san += toFile;
        san += toRank;
    }

    makeMove(move);
    auto legalMoves = generateLegalMovesImpl(1 - g_sideToMove);
    bool isCheckmate = legalMoves.empty() && isInCheck(1 - g_sideToMove);
    if (isCheckmate) san += "#";
    else if (isInCheck(1 - g_sideToMove)) san += "+";
    undoMove();

    return san;
}

Move stringToMove(const std::string& str) {
    if (str.length() < 4) return Move{0, 0, 0, 0, 0, 0, 0};
    int fromFile = str[0] - 'a';
    int fromRank = str[1] - '1';
    int toFile = str[2] - 'a';
    int toRank = str[3] - '1';
    int from = (fromRank << 4) | fromFile;
    int to = (toRank << 4) | toFile;
    int promotion = PIECE_NONE;
    if (str.length() > 4) {
        switch (str[4]) {
            case 'q': promotion = PIECE_QUEEN; break;
            case 'r': promotion = PIECE_ROOK; break;
            case 'b': promotion = PIECE_BISHOP; break;
            case 'n': promotion = PIECE_KNIGHT; break;
        }
    }

    auto legalMoves = generateLegalMovesImpl(g_sideToMove);
    for (const auto& move : legalMoves) {
        if (move.from == from && move.to == to && move.promotion == promotion) {
            return move;
        }
    }
    return Move{0, 0, 0, 0, 0, 0, 0};
}

}
