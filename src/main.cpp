#include "types.h"
#include "globals.h"
#include "uci.h"
#include "board.h"
#include "move.h"
#include "eval.h"
#include "perft.h"
#include "bench.h"
#include "search.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace Yuuki;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    initializeEngine();

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        if (tokens.empty()) continue;

        std::string command = tokens[0];

        if (command == "uci") {
            sendID();
            sendUCIOptions();
        } else if (command == "isready") {
            sendUCI("readyok");
        } else if (command == "ucinewgame") {
            handleNewGame();
        } else if (command == "setoption") {
            handleSetOption(tokens);
        } else if (command == "position") {
            handlePosition(line);
        } else if (command == "go") {
            handleGo(line);
        } else if (command == "stop") {
            g_stopSearch = true;
            g_isPondering = false;
        } else if (command == "quit") {
            g_stopSearch = true;
            if (g_searchThread.joinable()) {
                g_searchThread.join();
            }
            break;
        } else if (command == "ponderhit") {
            handlePonderHit();
        } else if (command == "debug") {
            if (tokens.size() > 1) g_debugMode = (tokens[1] == "on");
        } else if (command == "d") {
            printBoard();
        } else if (command == "eval") {
            int ev = evaluateFull();
            sendInfoString("Evaluation: " + std::to_string(ev) + " cp (side to move)");
        } else if (command == "perft") {
            int perftDepth = (tokens.size() > 1) ? std::stoi(tokens[1]) : 6;
            runPerft(perftDepth);
        } else if (command == "divide") {
            int divDepth = (tokens.size() > 1) ? std::stoi(tokens[1]) : 6;
            runDivide(divDepth);
        }
    }

    return 0;
}
