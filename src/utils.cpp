#include "utils.h"
#include "pstables.h"
#include <cctype>
#include <algorithm>

namespace Yuuki {

int getPieceValue(int pieceType) {
    switch (pieceType) {
        case PIECE_PAWN: return g_pieceValues.pawn;
        case PIECE_KNIGHT: return g_pieceValues.knight;
        case PIECE_BISHOP: return g_pieceValues.bishop;
        case PIECE_ROOK: return g_pieceValues.rook;
        case PIECE_QUEEN: return g_pieceValues.queen;
        case PIECE_KING: return g_pieceValues.king;
        default: return 0;
    }
}

int getPieceValueFromChar(char ch) {
    switch (std::tolower(ch)) {
        case 'p': return g_pieceValues.pawn;
        case 'n': return g_pieceValues.knight;
        case 'b': return g_pieceValues.bishop;
        case 'r': return g_pieceValues.rook;
        case 'q': return g_pieceValues.queen;
        case 'k': return g_pieceValues.king;
        default: return 0;
    }
}

int getPSTValue(int pieceType, int square, bool isEndgame) {
    int idx = squareRank(square) * 8 + squareFile(square);
    switch (pieceType) {
        case PIECE_PAWN: return isEndgame ? PST_EG_PAWN[idx] : PST_MG_PAWN[idx];
        case PIECE_KNIGHT: return isEndgame ? PST_EG_KNIGHT[idx] : PST_MG_KNIGHT[idx];
        case PIECE_BISHOP: return isEndgame ? PST_EG_BISHOP[idx] : PST_MG_BISHOP[idx];
        case PIECE_ROOK: return isEndgame ? PST_EG_ROOK[idx] : PST_MG_ROOK[idx];
        case PIECE_QUEEN: return isEndgame ? PST_EG_QUEEN[idx] : PST_MG_QUEEN[idx];
        case PIECE_KING: return isEndgame ? PST_EG_KING[idx] : PST_MG_KING[idx];
        default: return 0;
    }
}

int interpolateScore(int mgScore, int egScore) {
    int phase = g_phaseScore;
    if (phase > 24) phase = 24;
    return ((mgScore * phase) + (egScore * (24 - phase))) / 24;
}

void updateGamePhase() {
    int phase = 0;
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) {
            const Piece& p = g_board[i];
            if (p.piece != PIECE_NONE && p.piece != PIECE_PAWN && p.piece != PIECE_KING) {
                phase += PIECE_PHASE_VALUE[p.piece];
            }
        }
    }
    g_phaseScore = phase;
    if (phase > 18) {
        g_gamePhase = GAME_PHASE_OPENING;
    } else if (phase > 8) {
        g_gamePhase = GAME_PHASE_MIDDLEGAME;
    } else {
        g_gamePhase = GAME_PHASE_ENDGAME;
    }
}

uint64_t splitmix64(uint64_t& state) {
    uint64_t z = (state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

}
