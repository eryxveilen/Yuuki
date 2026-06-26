#include "board.h"
#include "globals.h"
#include "utils.h"
#include <sstream>
#include <cctype>

namespace Yuuki {

void initializeBoard() {
    for (int i = 0; i < 128; i++) {
        g_board[i] = {PIECE_NONE, 0};
    }

    int backRank[8] = {PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_QUEEN, PIECE_KING, PIECE_BISHOP, PIECE_KNIGHT, PIECE_ROOK};

    for (int file = 0; file < 8; file++) {
        g_board[file] = {static_cast<uint8_t>(backRank[file]), COLOR_WHITE};
        g_board[16 + file] = {PIECE_PAWN, COLOR_WHITE};
        g_board[112 + file] = {static_cast<uint8_t>(backRank[file]), COLOR_BLACK};
        g_board[96 + file] = {PIECE_PAWN, COLOR_BLACK};
    }

    g_sideToMove = COLOR_WHITE;
    g_castlingRights = CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ;
    g_enPassantSquare = -1;
    g_halfMoveClock = 0;
    g_fullMoveNumber = 1;
    g_phaseScore = 24;
    g_gamePhase = GAME_PHASE_OPENING;
    g_moveHistory.clear();
    g_positionHistory.clear();
}

void initZobrist() {
    uint64_t seed = 0x83D24E1F;
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 7; k++) {
                g_zobristKeys[i][j][k] = splitmix64(seed);
            }
        }
    }
    g_zobristSide = splitmix64(seed);
    for (int i = 0; i < 4; i++) {
        g_zobristCastling[i] = splitmix64(seed);
    }
    for (int i = 0; i < 128; i++) {
        g_zobristEnPassant[i] = splitmix64(seed);
    }
}

uint64_t computeZobristHash() {
    uint64_t hash = 0;
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) {
            const Piece& p = g_board[i];
            if (p.piece != PIECE_NONE) {
                hash ^= g_zobristKeys[i][p.color][p.piece];
            }
        }
    }
    if (g_sideToMove == COLOR_BLACK) {
        hash ^= g_zobristSide;
    }
    if (g_castlingRights & CASTLE_WK) hash ^= g_zobristCastling[0];
    if (g_castlingRights & CASTLE_WQ) hash ^= g_zobristCastling[1];
    if (g_castlingRights & CASTLE_BK) hash ^= g_zobristCastling[2];
    if (g_castlingRights & CASTLE_BQ) hash ^= g_zobristCastling[3];
    if (g_enPassantSquare != -1 && isValidSquare(g_enPassantSquare)) {
        hash ^= g_zobristEnPassant[g_enPassantSquare];
    }
    return hash;
}

uint64_t getPositionHash() {
    return computeZobristHash();
}

uint64_t getPawnHash() {
    uint64_t hash = 0;
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) {
            const Piece& p = g_board[i];
            if (p.piece == PIECE_PAWN) {
                hash ^= g_zobristKeys[i][p.color][PIECE_PAWN];
            }
        }
    }
    return hash;
}

void setFEN(const std::string& fen) {
    std::stringstream ss(fen);
    std::string placement, side, castling, ep;
    int halfmove = 0, fullmove = 1;

    ss >> placement >> side >> castling >> ep;
    if (ss >> halfmove) {}
    if (ss >> fullmove) {}

    for (int i = 0; i < 128; i++) {
        g_board[i] = {PIECE_NONE, 0};
    }

    int rank = 7;
    int file = 0;

    for (size_t i = 0; i < placement.length(); i++) {
        char ch = placement[i];
        if (ch == '/') {
            rank--;
            file = 0;
        } else if (ch >= '1' && ch <= '8') {
            file += (ch - '0');
        } else {
            int piece = PIECE_NONE;
            int color = COLOR_WHITE;
            char lower = std::tolower(ch);
            if (lower == 'p') piece = PIECE_PAWN;
            else if (lower == 'n') piece = PIECE_KNIGHT;
            else if (lower == 'b') piece = PIECE_BISHOP;
            else if (lower == 'r') piece = PIECE_ROOK;
            else if (lower == 'q') piece = PIECE_QUEEN;
            else if (lower == 'k') piece = PIECE_KING;
            if (ch >= 'a' && ch <= 'z') color = COLOR_BLACK;
            if (piece != PIECE_NONE) {
                g_board[makeSquare(file, rank)] = {static_cast<uint8_t>(piece), static_cast<uint8_t>(color)};
                file++;
            }
        }
    }

    g_sideToMove = (side == "w") ? COLOR_WHITE : COLOR_BLACK;
    g_castlingRights = 0;
    if (castling.find('K') != std::string::npos) g_castlingRights |= CASTLE_WK;
    if (castling.find('Q') != std::string::npos) g_castlingRights |= CASTLE_WQ;
    if (castling.find('k') != std::string::npos) g_castlingRights |= CASTLE_BK;
    if (castling.find('q') != std::string::npos) g_castlingRights |= CASTLE_BQ;

    g_enPassantSquare = -1;
    if (ep != "-" && ep.length() >= 2) {
        int epFile = ep[0] - 'a';
        int epRank = ep[1] - '1';
        g_enPassantSquare = makeSquare(epFile, epRank);
    }

    g_halfMoveClock = halfmove;
    g_fullMoveNumber = fullmove;
    updateGamePhase();
}

std::string getFEN() {
    std::string fen;
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;
        for (int file = 0; file < 8; file++) {
            int sq = makeSquare(file, rank);
            const Piece& piece = g_board[sq];
            if (piece.piece == PIECE_NONE) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                char ch = g_pieceToChar[piece.piece];
                if (piece.color == COLOR_WHITE) ch = std::toupper(ch);
                fen += ch;
            }
        }
        if (emptyCount > 0) fen += std::to_string(emptyCount);
        if (rank > 0) fen += "/";
    }

    fen += " ";
    fen += (g_sideToMove == COLOR_WHITE) ? "w" : "b";
    fen += " ";

    std::string castling;
    if (g_castlingRights & CASTLE_WK) castling += "K";
    if (g_castlingRights & CASTLE_WQ) castling += "Q";
    if (g_castlingRights & CASTLE_BK) castling += "k";
    if (g_castlingRights & CASTLE_BQ) castling += "q";
    if (castling.empty()) castling = "-";
    fen += castling;
    fen += " ";

    if (g_enPassantSquare != -1) {
        char file = 'a' + squareFile(g_enPassantSquare);
        char rank = '1' + squareRank(g_enPassantSquare);
        fen += file;
        fen += rank;
    } else {
        fen += "-";
    }
    fen += " ";
    fen += std::to_string(g_halfMoveClock);
    fen += " ";
    fen += std::to_string(g_fullMoveNumber);

    return fen;
}

}
