#ifndef YUUKI_MOVE_H
#define YUUKI_MOVE_H

#include "types.h"
#include <vector>

namespace Yuuki {

inline Move createMove(int from, int to, int piece, int captured = PIECE_NONE, int promotion = PIECE_NONE, int flags = FLAG_NONE) {
    return Move{static_cast<uint8_t>(from), static_cast<uint8_t>(to), static_cast<uint8_t>(piece),
                static_cast<uint8_t>(captured), static_cast<uint8_t>(promotion), static_cast<uint8_t>(flags), 0};
}

inline bool movesEqual(const Move& a, const Move& b) {
    return a.from == b.from && a.to == b.to && a.promotion == b.promotion;
}

void makeMove(const Move& move);
void undoMove();
void makeNullMove();
void undoNullMove();

std::vector<Move> generatePseudoLegalMoves(int color, bool capturesOnly);
std::vector<Move> generateLegalMovesImpl(int color);
std::vector<Move> generateCaptureMoves(int color);
std::vector<Move> generateEvasions(int color);

std::string moveToString(const Move& move);
std::string moveToSAN(const Move& move);
Move stringToMove(const std::string& str);

void generatePawnMoves(int from, int color, std::vector<Move>& moves, bool capturesOnly);
void generateKnightMoves(int from, int color, std::vector<Move>& moves, bool capturesOnly);
void generateSlidingMoves(int from, const int* deltas, int numDeltas, int color, std::vector<Move>& moves, bool capturesOnly);
void generateKingMoves(int from, int color, std::vector<Move>& moves, bool capturesOnly);

std::vector<int> getSquaresBetween(int sq1, int sq2);

}

#endif
