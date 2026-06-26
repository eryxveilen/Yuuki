#ifndef YUUKI_UTILS_H
#define YUUKI_UTILS_H

#include "types.h"
#include "globals.h"

namespace Yuuki {

inline bool isValidSquare(int sq) {
    return (sq & 0x88) == 0;
}

inline int squareFile(int sq) {
    return sq & 7;
}

inline int squareRank(int sq) {
    return sq >> 4;
}

inline int makeSquare(int file, int rank) {
    return (rank << 4) | file;
}

inline int mirrorSquare(int sq) {
    return makeSquare(squareFile(sq), 7 - squareRank(sq));
}

inline int squareDistance(int sq1, int sq2) {
    return std::max(std::abs(squareFile(sq1) - squareFile(sq2)), std::abs(squareRank(sq1) - squareRank(sq2)));
}

inline int manhattanDistance(int sq1, int sq2) {
    return std::abs(squareFile(sq1) - squareFile(sq2)) + std::abs(squareRank(sq1) - squareRank(sq2));
}

int getPieceValue(int pieceType);
int getPieceValueFromChar(char ch);

inline int getBlackPSTIndex(int sq) {
    int f = squareFile(sq);
    int r = squareRank(sq);
    return (7 - r) * 8 + f;
}

int getPSTValue(int pieceType, int square, bool isEndgame);
int interpolateScore(int mgScore, int egScore);
void updateGamePhase();

uint64_t splitmix64(uint64_t& state);

}

#endif
