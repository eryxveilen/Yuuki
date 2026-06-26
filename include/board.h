#ifndef YUUKI_BOARD_H
#define YUUKI_BOARD_H

#include "types.h"

namespace Yuuki {

void initializeBoard();
void initZobrist();
uint64_t computeZobristHash();
uint64_t getPositionHash();
uint64_t getPawnHash();
void setFEN(const std::string& fen);
std::string getFEN();

}

#endif
