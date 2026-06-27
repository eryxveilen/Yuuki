#ifndef YUUKI_ATTACK_H
#define YUUKI_ATTACK_H

#include "types.h"
#include <vector>

namespace Yuuki {

bool isSquareAttacked(int sq, int byColor);
int findKingSquare(int color);
bool isInCheck(int color);
bool pieceAttacksSquare(int fromSq, const Piece& piece, int toSq);
bool pieceCanMoveTo(int fromSq, const Piece& piece, int toSq);

std::vector<AttackerInfo> getAttackingPieces(int sq, int byColor);
int countAttackers(int sq, int byColor);

}

#endif
