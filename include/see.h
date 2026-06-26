#ifndef YUUKI_SEE_H
#define YUUKI_SEE_H

#include "types.h"

namespace Yuuki {

int see(int sq);
inline int seeCapture(const Move& move) { return see(move.to); }

}

#endif
