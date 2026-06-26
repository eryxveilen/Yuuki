#ifndef YUUKI_EVAL_H
#define YUUKI_EVAL_H

namespace Yuuki {

void clearEvaluationCache();
void clearPawnHashTable();

int evaluateMaterial();
int evaluatePST();
int evaluateMobility();
int evaluatePawnStructure();
int evaluatePawnShield(int kingSq, int color);
int evaluatePawnStorm(int kingSq, int color);
int evaluateKingSafety();
int evaluateRooks();
int evaluateKnights();
int evaluateBishops();
int evaluateThreats();
int evaluateSpace();
int evaluateKingTropism();
int evaluatePinnedPieces();
int evaluateMaterialImbalance();
int evaluateInitiative();

int evaluateFull();

inline int evaluate() {
    return evaluateFull();
}

}

#endif
