#include "perft.h"
#include "globals.h"
#include "utils.h"
#include "move.h"
#include "uci.h"
#include <chrono>

namespace Yuuki {

uint64_t perft(int depth) {
    if (depth == 0) return 1;
    auto moves = generateLegalMovesImpl(g_sideToMove);
    if (depth == 1) return moves.size();
    uint64_t nodes = 0;
    for (const auto& move : moves) {
        makeMove(move);
        nodes += perft(depth - 1);
        undoMove();
    }
    return nodes;
}

uint64_t divide(int depth) {
    auto moves = generateLegalMovesImpl(g_sideToMove);
    uint64_t total = 0;
    for (const auto& move : moves) {
        makeMove(move);
        uint64_t nodes = perft(depth - 1);
        undoMove();
        sendInfoString(moveToString(move) + ": " + std::to_string(nodes));
        total += nodes;
    }
    sendInfoString("Total: " + std::to_string(total));
    return total;
}

void runPerft(int depth) {
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perft(depth);
    auto end = std::chrono::steady_clock::now();
    int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    uint64_t nps = elapsed > 0 ? static_cast<uint64_t>(nodes / (elapsed / 1000.0)) : 0;
    sendInfoString("Perft(" + std::to_string(depth) + ") = " + std::to_string(nodes) + " (" + std::to_string(elapsed) + "ms, " + std::to_string(nps) + " nps)");
}

void runDivide(int depth) {
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = divide(depth);
    auto end = std::chrono::steady_clock::now();
    int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    sendInfoString("Divide completed in " + std::to_string(elapsed) + "ms, " + std::to_string(nodes) + " nodes");
}

}
