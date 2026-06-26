#ifndef YUUKI_GLOBALS_H
#define YUUKI_GLOBALS_H

#include "types.h"
#include <cstring>

namespace Yuuki {

extern std::array<Piece, 128> g_board;
extern int g_sideToMove;
extern int g_castlingRights;
extern int g_enPassantSquare;
extern int g_halfMoveClock;
extern int g_fullMoveNumber;
extern int g_phaseScore;
extern int g_gamePhase;

extern std::vector<UndoInfo> g_moveHistory;
extern std::vector<uint64_t> g_positionHistory;

extern PieceValues g_pieceValues;
extern EngineOptions g_engineOptions;

extern std::vector<Move> g_currentPV;
extern int g_currentScore;
extern int g_currentDepth;
extern int g_selDepth;
extern std::chrono::steady_clock::time_point g_searchStartTime;
extern uint64_t g_searchNodes;
extern std::atomic<bool> g_stopSearch;
extern std::atomic<bool> g_isSearching;
extern std::atomic<bool> g_isPondering;
extern std::atomic<bool> g_ponderHit;
extern bool g_debugMode;
extern Move g_lastBestMove;

extern std::array<std::array<Move, 2>, MAX_PLY> g_killerMoves;
extern int g_historyTable[2][128][128];
extern Move g_counterMoves[2][128][128];
extern bool g_counterMovesValid[2][128][128];

extern std::vector<TTEntry> g_transpositionTable;
extern size_t g_ttSize;

extern uint64_t g_zobristKeys[128][2][7];
extern uint64_t g_zobristSide;
extern uint64_t g_zobristCastling[4];
extern uint64_t g_zobristEnPassant[128];

extern std::unordered_map<uint64_t, int> g_pawnHashTable;
extern std::unordered_map<uint64_t, int> g_evalCache;

extern std::string g_pieceToChar;

extern int LMR_REDUCTION_TABLE[64][64];
extern int FUTILITY_MARGIN_TABLE[10];
extern int RAZOR_MARGIN_TABLE[4];

extern std::unordered_map<uint64_t, std::vector<BookEntry>> g_openingBook;
extern bool g_useOpeningBook;

extern SearchStats g_searchStats;
extern std::array<SearchStackEntry, MAX_PLY> g_searchStack;

extern int g_bestMoveChanges;
extern Move g_previousBestMove;
extern int g_stableBestMoveCount;

extern std::mutex g_uciMutex;
extern std::thread g_searchThread;

}

#endif
