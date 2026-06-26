#include "uci.h"
#include "globals.h"
#include "types.h"
#include "utils.h"
#include "board.h"
#include "move.h"
#include "eval.h"
#include "search.h"
#include "tt.h"
#include <iostream>
#include <sstream>
#include <thread>

namespace Yuuki {

void sendUCI(const std::string& msg) {
    std::lock_guard<std::mutex> lock(g_uciMutex);
    std::cout << msg << std::endl;
}

void sendInfoString(const std::string& str) {
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            sendUCI("info string " + line);
        }
    }
}

void sendBestMove(const Move& move, const Move& ponder) {
    if (move.from != 0 || move.to != 0) {
        std::string msg = "bestmove ";
        char fromFile = 'a' + (move.from & 7);
        char fromRank = '1' + (move.from >> 4);
        char toFile = 'a' + (move.to & 7);
        char toRank = '1' + (move.to >> 4);
        msg += fromFile; msg += fromRank; msg += toFile; msg += toRank;
        if (move.promotion != PIECE_NONE) {
            msg += g_pieceToChar[move.promotion];
        }
        if ((ponder.from != 0 || ponder.to != 0) && g_engineOptions.ponderEnabled) {
            msg += " ponder ";
            char pFromFile = 'a' + (ponder.from & 7);
            char pFromRank = '1' + (ponder.from >> 4);
            char pToFile = 'a' + (ponder.to & 7);
            char pToRank = '1' + (ponder.to >> 4);
            msg += pFromFile; msg += pFromRank; msg += pToFile; msg += pToRank;
            if (ponder.promotion != PIECE_NONE) {
                msg += g_pieceToChar[ponder.promotion];
            }
        }
        sendUCI(msg);
    } else {
        sendUCI("bestmove 0000");
    }
}

void sendInfo(const std::string& infoStr) {
    sendUCI("info " + infoStr);
}

void sendID() {
    sendUCI("id name " + std::string(ENGINE_NAME) + " " + ENGINE_VERSION);
    sendUCI("id author " + std::string(ENGINE_AUTHOR));
}

void sendUCIOptions() {
    sendUCI("option name Hash type spin default 64 min 1 max 4096");
    sendUCI("option name Clear Hash type button");
    sendUCI("option name Ponder type check default false");
    sendUCI("option name MultiPV type spin default 1 min 1 max 500");
    sendUCI("option name Skill Level type spin default 20 min 0 max 20");
    sendUCI("option name Move Overhead type spin default 30 min 0 max 5000");
    sendUCI("option name Minimum Thinking Time type spin default 20 min 0 max 5000");
    sendUCI("option name Slow Mover type spin default 100 min 10 max 1000");
    sendUCI("option name Threads type spin default 1 min 1 max 512");
    sendUCI("option name UCI_AnalyseMode type check default false");
    sendUCI("option name UCI_LimitStrength type check default false");
    sendUCI("option name UCI_Elo type spin default 3200 min 500 max 3200");
    sendUCI("option name UCI_ShowCurrLine type check default false");
    sendUCI("option name Contempt type spin default 0 min -100 max 100");
    sendUCI("option name Analysis Contempt type combo default Both var Off var White var Black var Both");
    sendUCI("option name PawnValue type spin default 100 min 0 max 2000");
    sendUCI("option name KnightValue type spin default 320 min 0 max 2000");
    sendUCI("option name BishopValue type spin default 330 min 0 max 2000");
    sendUCI("option name RookValue type spin default 500 min 0 max 2000");
    sendUCI("option name QueenValue type spin default 950 min 0 max 4000");
    sendUCI("option name KingValue type spin default 20000 min 0 max 100000");
    sendUCI("option name NullMoveReduction type spin default 3 min 1 max 10");
    sendUCI("option name NullMoveDepthLimit type spin default 3 min 1 max 20");
    sendUCI("option name LMRDepthThreshold type spin default 3 min 1 max 20");
    sendUCI("option name LMRBaseReduction type spin default 75 min 0 max 200");
    sendUCI("option name LMRMoveCountThreshold type spin default 3 min 1 max 20");
    sendUCI("option name AspirationWindow type spin default 25 min 1 max 500");
    sendUCI("option name AspirationWindowMinDepth type spin default 4 min 1 max 20");
    sendUCI("option name FutilityMargin type spin default 100 min 0 max 1000");
    sendUCI("option name FutilityDepthLimit type spin default 3 min 0 max 10");
    sendUCI("option name RazorMargin type spin default 300 min 0 max 1000");
    sendUCI("option name RazorDepthLimit type spin default 2 min 0 max 10");
    sendUCI("option name SingularExtensionMargin type spin default 50 min 0 max 500");
    sendUCI("option name SingularExtensionDepth type spin default 6 min 1 max 20");
    sendUCI("option name KingSafetyWeight type spin default 100 min 0 max 500");
    sendUCI("option name MobilityWeight type spin default 8 min 0 max 100");
    sendUCI("option name PawnStructureWeight type spin default 12 min 0 max 200");
    sendUCI("option name PassedPawnWeight type spin default 50 min 0 max 200");
    sendUCI("option name IsolatedPawnPenalty type spin default 15 min 0 max 100");
    sendUCI("option name DoubledPawnPenalty type spin default 10 min 0 max 100");
    sendUCI("option name BackwardPawnPenalty type spin default 12 min 0 max 100");
    sendUCI("option name BishopPairBonus type spin default 30 min 0 max 100");
    sendUCI("option name RookOnOpenFile type spin default 20 min 0 max 100");
    sendUCI("option name RookOnSemiOpenFile type spin default 10 min 0 max 100");
    sendUCI("option name RookOnSeventhRank type spin default 20 min 0 max 100");
    sendUCI("option name KnightOutpostBonus type spin default 15 min 0 max 100");
    sendUCI("option name KnightOnRimPenalty type spin default 5 min 0 max 50");
    sendUCI("option name TempoBonus type spin default 10 min 0 max 100");
    sendUCI("option name SpaceWeight type spin default 2 min 0 max 50");
    sendUCI("option name ThreatWeight type spin default 5 min 0 max 50");
    sendUCI("option name TrappedPiecePenalty type spin default 50 min 0 max 200");
    sendUCI("option name OverextendedPawnPenalty type spin default 8 min 0 max 50");
    sendUCI("option name PawnChainBonus type spin default 8 min 0 max 50");
    sendUCI("option name CentralPawnBonus type spin default 15 min 0 max 100");
    sendUCI("option name AdvancedPawnBonus type spin default 10 min 0 max 100");
    sendUCI("option name KingTropismWeight type spin default 3 min 0 max 30");
    sendUCI("option name PawnShieldWeight type spin default 20 min 0 max 100");
    sendUCI("option name PawnStormWeight type spin default 15 min 0 max 100");
    sendUCI("option name AttackZoneWeight type spin default 10 min 0 max 100");
    sendUCI("option name QueenEarlyDevelopmentPenalty type spin default 10 min 0 max 100");
    sendUCI("option name RookCoordinationBonus type spin default 5 min 0 max 50");
    sendUCI("option name MinorBehindPawnBonus type spin default 5 min 0 max 50");
    sendUCI("option name BadBishopPenalty type spin default 8 min 0 max 50");
    sendUCI("option name PinnedPiecePenalty type spin default 10 min 0 max 100");
    sendUCI("option name DiscoveryThreatBonus type spin default 15 min 0 max 100");
    sendUCI("option name MaterialImbalanceWeight type spin default 5 min 0 max 50");
    sendUCI("option name InitiativeWeight type spin default 5 min 0 max 50");
    sendUCI("option name DrawScore type spin default 0 min -100 max 100");
    sendUCI("option name VerboseUCI type check default false");
    sendUCI("option name OwnBook type check default false");
    sendUCI("option name BookFile type string default");
    sendUCI("option name BestBookMove type check default false");
    sendUCI("option name Clean Search type check default true");
    sendUCI("option name SelectiveDepth type spin default 64 min 1 max 128");
    sendUCI("uciok");
}

void handleSetOption(const std::vector<std::string>& tokens) {
    std::string name;
    std::string value;
    bool foundValue = false;

    for (size_t i = 1; i < tokens.size(); i++) {
        if (tokens[i] == "name") {
            i++;
            while (i < tokens.size() && tokens[i] != "value") {
                name += (name.empty() ? "" : " ") + tokens[i];
                i++;
            }
            i--;
        } else if (tokens[i] == "value") {
            foundValue = true;
            i++;
            while (i < tokens.size()) {
                value += (value.empty() ? "" : " ") + tokens[i];
                i++;
            }
        }
    }

    if (!foundValue && name != "Clear Hash") return;

    auto clearEval = []() { clearEvaluationCache(); };

    if (name == "Hash") {
        g_engineOptions.hashSizeMB = std::stoi(value);
        resizeTranspositionTable();
    } else if (name == "Clear Hash") {
        clearTranspositionTable();
    } else if (name == "Ponder") {
        g_engineOptions.ponderEnabled = (value == "true");
    } else if (name == "MultiPV") {
        g_engineOptions.multipv = std::stoi(value);
    } else if (name == "Skill Level") {
        g_engineOptions.skillLevel = std::stoi(value);
        if (g_engineOptions.skillLevel < 0) g_engineOptions.skillLevel = 0;
        if (g_engineOptions.skillLevel > 20) g_engineOptions.skillLevel = 20;
    } else if (name == "Move Overhead") {
        g_engineOptions.moveOverhead = std::stoi(value);
    } else if (name == "Minimum Thinking Time") {
        g_engineOptions.minimumThinkingTime = std::stoi(value);
    } else if (name == "Slow Mover") {
        g_engineOptions.slowMover = std::stoi(value);
    } else if (name == "Threads") {
        g_engineOptions.threads = std::stoi(value);
    } else if (name == "UCI_AnalyseMode") {
        g_engineOptions.analyzeMode = (value == "true");
    } else if (name == "UCI_ShowCurrLine") {
        g_engineOptions.showCurrentLine = (value == "true");
    } else if (name == "Contempt") {
        g_engineOptions.contempt = std::stoi(value);
    } else if (name == "PawnValue") {
        g_pieceValues.pawn = std::stoi(value); clearEval();
    } else if (name == "KnightValue") {
        g_pieceValues.knight = std::stoi(value); clearEval();
    } else if (name == "BishopValue") {
        g_pieceValues.bishop = std::stoi(value); clearEval();
    } else if (name == "RookValue") {
        g_pieceValues.rook = std::stoi(value); clearEval();
    } else if (name == "QueenValue") {
        g_pieceValues.queen = std::stoi(value); clearEval();
    } else if (name == "KingValue") {
        g_pieceValues.king = std::stoi(value); clearEval();
    } else if (name == "NullMoveReduction") {
        g_engineOptions.nullMoveReduction = std::stoi(value);
    } else if (name == "NullMoveDepthLimit") {
        g_engineOptions.nullMoveDepthLimit = std::stoi(value);
    } else if (name == "LMRDepthThreshold") {
        g_engineOptions.lmrDepthThreshold = std::stoi(value);
    } else if (name == "LMRBaseReduction") {
        g_engineOptions.lmrBaseReduction = std::stoi(value) / 100.0;
        buildLMRTable();
    } else if (name == "LMRMoveCountThreshold") {
        g_engineOptions.lmrMoveCountThreshold = std::stoi(value);
    } else if (name == "AspirationWindow") {
        g_engineOptions.aspirationWindow = std::stoi(value);
    } else if (name == "AspirationWindowMinDepth") {
        g_engineOptions.aspirationWindowMinDepth = std::stoi(value);
    } else if (name == "FutilityMargin") {
        g_engineOptions.futilityMargin = std::stoi(value);
        buildFutilityMargins();
    } else if (name == "FutilityDepthLimit") {
        g_engineOptions.futilityDepthLimit = std::stoi(value);
    } else if (name == "RazorMargin") {
        g_engineOptions.razorMargin = std::stoi(value);
        buildRazorMargins();
    } else if (name == "SingularExtensionMargin") {
        g_engineOptions.singularExtensionMargin = std::stoi(value);
    } else if (name == "SingularExtensionDepth") {
        g_engineOptions.singularExtensionDepth = std::stoi(value);
    } else if (name == "KingSafetyWeight") {
        g_engineOptions.kingSafetyWeight = std::stoi(value); clearEval();
    } else if (name == "MobilityWeight") {
        g_engineOptions.mobilityWeight = std::stoi(value); clearEval();
    } else if (name == "PawnStructureWeight") {
        g_engineOptions.pawnStructureWeight = std::stoi(value); clearEval();
    } else if (name == "PassedPawnWeight") {
        g_engineOptions.passedPawnWeight = std::stoi(value); clearEval();
    } else if (name == "IsolatedPawnPenalty") {
        g_engineOptions.isolatedPawnPenalty = std::stoi(value); clearEval();
    } else if (name == "DoubledPawnPenalty") {
        g_engineOptions.doubledPawnPenalty = std::stoi(value); clearEval();
    } else if (name == "BackwardPawnPenalty") {
        g_engineOptions.backwardPawnPenalty = std::stoi(value); clearEval();
    } else if (name == "BishopPairBonus") {
        g_engineOptions.bishopPairBonus = std::stoi(value); clearEval();
    } else if (name == "RookOnOpenFile") {
        g_engineOptions.rookOnOpenFile = std::stoi(value); clearEval();
    } else if (name == "RookOnSemiOpenFile") {
        g_engineOptions.rookOnSemiOpenFile = std::stoi(value); clearEval();
    } else if (name == "RookOnSeventhRank") {
        g_engineOptions.rookOnSeventhRank = std::stoi(value); clearEval();
    } else if (name == "KnightOutpostBonus") {
        g_engineOptions.knightOutpostBonus = std::stoi(value); clearEval();
    } else if (name == "KnightOnRimPenalty") {
        g_engineOptions.knightOnRimPenalty = std::stoi(value); clearEval();
    } else if (name == "TempoBonus") {
        g_engineOptions.tempoBonus = std::stoi(value); clearEval();
    } else if (name == "SpaceWeight") {
        g_engineOptions.spaceWeight = std::stoi(value); clearEval();
    } else if (name == "ThreatWeight") {
        g_engineOptions.threatWeight = std::stoi(value); clearEval();
    } else if (name == "TrappedPiecePenalty") {
        g_engineOptions.trappedPiecePenalty = std::stoi(value); clearEval();
    } else if (name == "OverextendedPawnPenalty") {
        g_engineOptions.overextendedPawnPenalty = std::stoi(value); clearEval();
    } else if (name == "PawnChainBonus") {
        g_engineOptions.pawnChainBonus = std::stoi(value); clearEval();
    } else if (name == "CentralPawnBonus") {
        g_engineOptions.centralPawnBonus = std::stoi(value); clearEval();
    } else if (name == "AdvancedPawnBonus") {
        g_engineOptions.advancedPawnBonus = std::stoi(value); clearEval();
    } else if (name == "KingTropismWeight") {
        g_engineOptions.kingTropismWeight = std::stoi(value); clearEval();
    } else if (name == "PawnShieldWeight") {
        g_engineOptions.pawnShieldWeight = std::stoi(value); clearEval();
    } else if (name == "PawnStormWeight") {
        g_engineOptions.pawnStormWeight = std::stoi(value); clearEval();
    } else if (name == "AttackZoneWeight") {
        g_engineOptions.attackZoneWeight = std::stoi(value); clearEval();
    } else if (name == "QueenEarlyDevelopmentPenalty") {
        g_engineOptions.queenEarlyDevelopmentPenalty = std::stoi(value); clearEval();
    } else if (name == "RookCoordinationBonus") {
        g_engineOptions.rookCoordinationBonus = std::stoi(value); clearEval();
    } else if (name == "MinorBehindPawnBonus") {
        g_engineOptions.minorBehindPawnBonus = std::stoi(value); clearEval();
    } else if (name == "BadBishopPenalty") {
        g_engineOptions.badBishopPenalty = std::stoi(value); clearEval();
    } else if (name == "PinnedPiecePenalty") {
        g_engineOptions.pinnedPiecePenalty = std::stoi(value); clearEval();
    } else if (name == "DiscoveryThreatBonus") {
        g_engineOptions.discoveryThreatBonus = std::stoi(value); clearEval();
    } else if (name == "MaterialImbalanceWeight") {
        g_engineOptions.materialImbalanceWeight = std::stoi(value); clearEval();
    } else if (name == "InitiativeWeight") {
        g_engineOptions.initiativeWeight = std::stoi(value); clearEval();
    } else if (name == "DrawScore") {
        g_engineOptions.drawScore = std::stoi(value);
    } else if (name == "VerboseUCI") {
        g_engineOptions.verboseUCI = (value == "true");
    } else if (name == "OwnBook") {
        g_useOpeningBook = (value == "true");
    } else if (name == "Clean Search") {
        g_engineOptions.cleanSearch = (value == "true");
    } else if (name == "SelectiveDepth") {
        g_engineOptions.selectiveDepth = std::stoi(value);
    }

    if (g_debugMode) {
        sendInfoString("Set " + name + " to " + value);
    }
}

void handlePosition(const std::string& cmd) {
    std::stringstream ss(cmd);
    std::string token;
    ss >> token;
    ss >> token;

    if (token == "startpos") {
        setFEN(INITIAL_FEN);
    } else if (token == "fen") {
        std::string fen;
        std::string part;
        while (ss >> part) {
            if (part == "moves") break;
            fen += (fen.empty() ? "" : " ") + part;
        }
        setFEN(fen);
        token = part;
    }

    g_positionHistory.clear();

    if (token == "moves" || (ss >> token && token == "moves")) {
        std::string moveStr;
        while (ss >> moveStr) {
            Move move = stringToMove(moveStr);
            if (move.from != 0 || move.to != 0) {
                makeMove(move);
                g_positionHistory.push_back(getPositionHash());
            }
        }
    }
}

void handleGo(const std::string& cmd) {
    std::stringstream ss(cmd);
    std::string token;
    ss >> token;

    int depth = 64;
    int timeLimit = -1;
    int wtime = -1, btime = -1;
    int winc = 0, binc = 0;
    int movesToGo = -1;
    int moveTime = -1;
    bool infinite = false;
    bool ponder = false;

    while (ss >> token) {
        if (token == "depth") { ss >> depth; }
        else if (token == "wtime") { ss >> wtime; }
        else if (token == "btime") { ss >> btime; }
        else if (token == "winc") { ss >> winc; }
        else if (token == "binc") { ss >> binc; }
        else if (token == "movestogo") { ss >> movesToGo; }
        else if (token == "movetime") { ss >> moveTime; }
        else if (token == "infinite") { infinite = true; depth = 128; }
        else if (token == "ponder") { ponder = true; }
    }

    if (moveTime != -1) {
        timeLimit = moveTime;
    } else if (timeLimit == -1 && !infinite) {
        int myTime = (g_sideToMove == COLOR_WHITE) ? wtime : btime;
        int myInc = (g_sideToMove == COLOR_WHITE) ? winc : binc;

        if (myTime != -1) {
            int baseTime = static_cast<int>(myTime * (g_engineOptions.slowMover / 100.0));
            if (movesToGo > 0) {
                timeLimit = baseTime / movesToGo + myInc;
            } else {
                timeLimit = baseTime / 30 + myInc;
            }
            int maxTime = static_cast<int>(myTime * 0.4);
            if (timeLimit > maxTime) timeLimit = maxTime;
            if (timeLimit < g_engineOptions.minimumThinkingTime) timeLimit = g_engineOptions.minimumThinkingTime;
            if (myTime > 0 && timeLimit > myTime - g_engineOptions.emergencyTimeBuffer) {
                timeLimit = std::max(100, myTime - g_engineOptions.emergencyTimeBuffer);
            }
        } else {
            timeLimit = 5000;
        }
    }

    if (g_engineOptions.skillLevel < 20 && !infinite) {
        depth = std::max(1, static_cast<int>(depth * g_engineOptions.skillLevel / 20.0));
    }

    if (g_engineOptions.analyzeMode) {
        infinite = true;
        g_engineOptions.ponderEnabled = false;
    }

    if (ponder && g_engineOptions.ponderEnabled) {
        g_isPondering = true;
        g_ponderHit = false;
    }

    if (g_searchThread.joinable()) {
        g_stopSearch = true;
        g_searchThread.join();
    }

    g_stopSearch = false;
    g_searchThread = std::thread([depth, timeLimit, infinite]() {
        Move bestMove = iterativeDeepening(depth, timeLimit, infinite);
        Move ponderMove{0, 0, 0, 0, 0, 0, 0};
        if (bestMove.from != 0 || bestMove.to != 0) {
            makeMove(bestMove);
            auto legalMoves = generateLegalMovesImpl(g_sideToMove);
            if (!legalMoves.empty()) {
                orderMoves(legalMoves, getPositionHash(), 0);
                ponderMove = legalMoves[0];
            }
            undoMove();
        }
        g_isSearching = false;
        g_isPondering = false;
        sendBestMove(bestMove, ponderMove);
    });
}

void handlePonderHit() {
    g_isPondering = false;
    g_ponderHit = true;
}

void printBoard() {
    std::string output = "\n";
    for (int rank = 7; rank >= 0; rank--) {
        output += std::to_string(rank + 1) + "  ";
        for (int file = 0; file < 8; file++) {
            int sq = makeSquare(file, rank);
            const Piece& piece = g_board[sq];
            char ch = '.';
            if (piece.piece != PIECE_NONE) {
                ch = g_pieceToChar[piece.piece];
                if (piece.color == COLOR_WHITE) ch = std::toupper(ch);
            }
            output += ch;
            output += " ";
        }
        output += "\n";
    }
    output += "\n   a b c d e f g h\n";
    output += "FEN: " + getFEN() + "\n";
    output += "Side: " + std::string(g_sideToMove == COLOR_WHITE ? "White" : "Black") + "\n";
    output += "Phase: " + std::to_string(g_phaseScore) + " (" + (g_gamePhase == GAME_PHASE_OPENING ? "opening" : g_gamePhase == GAME_PHASE_MIDDLEGAME ? "middlegame" : "endgame") + ")\n";
    std::stringstream hashss;
    hashss << std::hex << getPositionHash();
    output += "Hash: " + hashss.str() + "\n";
    output += "Eval: " + std::to_string(evaluate()) + "\n";
    sendInfoString(output);
}

void initializeEngine() {
    initializeBoard();
    initZobrist();
    buildLMRTable();
    buildFutilityMargins();
    buildRazorMargins();
    resizeTranspositionTable();
    updateGamePhase();
    resetSearchState();
    sendInfoString(std::string(ENGINE_NAME) + " v" + ENGINE_VERSION + " by " + ENGINE_AUTHOR + " initialized");
}

}
