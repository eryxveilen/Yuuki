#include "globals.h"

namespace Yuuki {

std::array<Piece, 128> g_board;
int g_sideToMove = COLOR_WHITE;
int g_castlingRights = 0;
int g_enPassantSquare = -1;
int g_halfMoveClock = 0;
int g_fullMoveNumber = 1;
int g_phaseScore = 0;
int g_gamePhase = GAME_PHASE_OPENING;

std::vector<UndoInfo> g_moveHistory;
std::vector<uint64_t> g_positionHistory;

PieceValues g_pieceValues;
EngineOptions g_engineOptions;

std::vector<Move> g_currentPV;
int g_currentScore = 0;
int g_currentDepth = 0;
int g_selDepth = 0;
std::chrono::steady_clock::time_point g_searchStartTime;
uint64_t g_searchNodes = 0;
std::atomic<bool> g_stopSearch{false};
std::atomic<bool> g_isSearching{false};
std::atomic<bool> g_isPondering{false};
std::atomic<bool> g_ponderHit{false};
bool g_debugMode = false;
Move g_lastBestMove;

std::array<std::array<Move, 2>, MAX_PLY> g_killerMoves;
int g_historyTable[2][128][128];
Move g_counterMoves[2][128][128];
bool g_counterMovesValid[2][128][128];

std::vector<TTEntry> g_transpositionTable;
size_t g_ttSize = 0;

uint64_t g_zobristKeys[128][2][7];
uint64_t g_zobristSide;
uint64_t g_zobristCastling[4];
uint64_t g_zobristEnPassant[128];

std::unordered_map<uint64_t, int> g_pawnHashTable;
std::unordered_map<uint64_t, int> g_evalCache;

std::string g_pieceToChar = ".pnbrqk";

int LMR_REDUCTION_TABLE[64][64];
int FUTILITY_MARGIN_TABLE[10];
int RAZOR_MARGIN_TABLE[4];

std::unordered_map<uint64_t, std::vector<BookEntry>> g_openingBook;
bool g_useOpeningBook = false;

SearchStats g_searchStats;
std::array<SearchStackEntry, MAX_PLY> g_searchStack;

int g_bestMoveChanges = 0;
Move g_previousBestMove;
int g_stableBestMoveCount = 0;

std::mutex g_uciMutex;
std::thread g_searchThread;

}
