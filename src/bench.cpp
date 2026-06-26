#include "bench.h"
#include "globals.h"
#include "types.h"
#include "board.h"
#include "move.h"
#include "eval.h"
#include "search.h"
#include "perft.h"
#include "uci.h"
#include <vector>
#include <chrono>
#include <cmath>

namespace Yuuki {

bool selfTest() {
    int passed = 0;
    int failed = 0;

    setFEN(INITIAL_FEN);
    auto moves = generateLegalMovesImpl(COLOR_WHITE);
    if (moves.size() == 20) passed++; else { failed++; sendInfoString("FAIL: Initial position moves=" + std::to_string(moves.size()) + ", expected 20"); }

    uint64_t p5 = perft(5);
    if (p5 == 4865609) passed++; else { failed++; sendInfoString("FAIL: Perft(5)=" + std::to_string(p5) + ", expected 4865609"); }

    setFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    auto castleMoves = generateLegalMovesImpl(COLOR_WHITE);
    bool hasKingside = false, hasQueenside = false;
    for (const auto& m : castleMoves) {
        if (m.flags & FLAG_CASTLING) {
            if (m.to > m.from) hasKingside = true;
            else hasQueenside = true;
        }
    }
    if (hasKingside && hasQueenside) passed++; else { failed++; sendInfoString("FAIL: Castling moves"); }

    setFEN("rnbqkbnr/pppppppp/8/8/4Pp2/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    auto epMoves = generateLegalMovesImpl(COLOR_BLACK);
    bool hasEP = false;
    for (const auto& m : epMoves) {
        if (m.flags & FLAG_ENPASSANT) hasEP = true;
    }
    if (hasEP) passed++; else { failed++; sendInfoString("FAIL: En passant detection"); }

    setFEN("8/8/8/8/8/8/PPPPPPPP/8 w - - 0 1");
    auto promoMoves = generateLegalMovesImpl(COLOR_WHITE);
    int promoCount = 0;
    for (const auto& m : promoMoves) {
        if (m.flags & FLAG_PROMOTION) promoCount++;
    }
    if (promoCount == 8) passed++; else { failed++; sendInfoString("FAIL: Promotion moves=" + std::to_string(promoCount) + ", expected 8"); }

    setFEN(INITIAL_FEN);
    int evalScore = evaluate();
    if (std::abs(evalScore) < 50) passed++; else { failed++; sendInfoString("FAIL: Initial eval=" + std::to_string(evalScore)); }

    setFEN("rnb1kbnr/pppp1ppp/8/4p3/5PPq/8/PPPPP2P/RNBQKBNR w KQkq - 0 1");
    auto checkmateMoves = generateLegalMovesImpl(COLOR_WHITE);
    if (checkmateMoves.empty() && isInCheck(COLOR_WHITE)) passed++; else { failed++; sendInfoString("FAIL: Checkmate detection"); }

    setFEN(INITIAL_FEN);
    std::string fen = getFEN();
    if (fen.substr(0, fen.find(' ')) == std::string(INITIAL_FEN).substr(0, std::string(INITIAL_FEN).find(' '))) passed++; else { failed++; sendInfoString("FAIL: FEN export"); }

    sendInfoString("Self-test: " + std::to_string(passed) + " passed, " + std::to_string(failed) + " failed");
    return failed == 0;
}

void runBenchmark() {
    std::vector<std::string> positions = {
        INITIAL_FEN,
        "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1",
        "rnbqkb1r/pppp1ppp/4pn2/8/2PP4/8/PP2PPPP/RNBQKBNR w KQkq - 0 1",
        "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 1",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 1",
        "rn1qkb1r/pp2pppp/2p2n2/5b2/P1BP4/2N2N2/1PP2PPP/R1BQK2R b KQkq - 0 1"
    };

    uint64_t totalNodes = 0;
    int totalTime = 0;

    for (size_t p = 0; p < positions.size(); p++) {
        setFEN(positions[p]);
        resetSearchState();
        auto start = std::chrono::steady_clock::now();
        Move bm = iterativeDeepening(8, 5000, false);
        auto end = std::chrono::steady_clock::now();
        int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        totalNodes += g_searchNodes;
        totalTime += elapsed;
        sendInfoString("Bench " + std::to_string(p + 1) + "/" + std::to_string(positions.size()) + ": " + std::to_string(elapsed) + "ms, " + std::to_string(g_searchNodes) + " nodes, bm=" + moveToString(bm));
    }

    uint64_t avgNps = totalTime > 0 ? static_cast<uint64_t>(totalNodes / (totalTime / 1000.0)) : 0;
    sendInfoString("Benchmark complete: " + std::to_string(totalNodes) + " nodes in " + std::to_string(totalTime) + "ms (" + std::to_string(avgNps) + " nps)");
}

}
