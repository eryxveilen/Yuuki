#ifndef YUUKI_PERFT_H
#define YUUKI_PERFT_H

#include "types.h"
#include <cstdint>

namespace Yuuki {

uint64_t perft(int depth);
uint64_t divide(int depth);
void runPerft(int depth);
void runDivide(int depth);

}

#endif
