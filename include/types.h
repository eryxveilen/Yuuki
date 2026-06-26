#ifndef YUUKI_TYPES_H
#define YUUKI_TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>

namespace Yuuki {

constexpr char ENGINE_NAME[] = "Yuuki";
constexpr char ENGINE_VERSION[] = "1.0";
constexpr char ENGINE_AUTHOR[] = "Eryx";

enum PieceType {
    PIECE_NONE = 0,
    PIECE_PAWN = 1,
    PIECE_KNIGHT = 2,
    PIECE_BISHOP = 3,
    PIECE_ROOK = 4,
    PIECE_QUEEN = 5,
    PIECE_KING = 6
};

enum Color {
    COLOR_WHITE = 0,
    COLOR_BLACK = 1
};

enum Castling {
    CASTLE_WK = 1,
    CASTLE_WQ = 2,
    CASTLE_BK = 4,
    CASTLE_BQ = 8
};

enum MoveFlags {
    FLAG_NONE = 0,
    FLAG_ENPASSANT = 1,
    FLAG_CASTLING = 2,
    FLAG_PAWN_DOUBLE = 4,
    FLAG_PROMOTION = 8
};

enum TTFlag {
    TT_EXACT = 0,
    TT_ALPHA = 1,
    TT_BETA = 2
};

enum GamePhase {
    GAME_PHASE_OPENING = 0,
    GAME_PHASE_MIDDLEGAME = 1,
    GAME_PHASE_ENDGAME = 2
};

constexpr int VALUE_ZERO = 0;
constexpr int VALUE_PAWN = 100;
constexpr int VALUE_KNIGHT = 320;
constexpr int VALUE_BISHOP = 330;
constexpr int VALUE_ROOK = 500;
constexpr int VALUE_QUEEN = 950;
constexpr int VALUE_KING = 20000;
constexpr int VALUE_INFINITE = 999999;
constexpr int VALUE_MATE = 90000;
constexpr int VALUE_DRAW = 0;

constexpr int MAX_PLY = 128;
constexpr int MAX_MOVES = 256;

constexpr char INITIAL_FEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum Squares : int {
    SQUARE_A1 = 0,   SQUARE_B1 = 1,   SQUARE_C1 = 2,   SQUARE_D1 = 3,
    SQUARE_E1 = 4,   SQUARE_F1 = 5,   SQUARE_G1 = 6,   SQUARE_H1 = 7,
    SQUARE_A2 = 16,  SQUARE_B2 = 17,  SQUARE_C2 = 18,  SQUARE_D2 = 19,
    SQUARE_E2 = 20,  SQUARE_F2 = 21,  SQUARE_G2 = 22,  SQUARE_H2 = 23,
    SQUARE_A3 = 32,  SQUARE_B3 = 33,  SQUARE_C3 = 34,  SQUARE_D3 = 35,
    SQUARE_E3 = 36,  SQUARE_F3 = 37,  SQUARE_G3 = 38,  SQUARE_H3 = 39,
    SQUARE_A4 = 48,  SQUARE_B4 = 49,  SQUARE_C4 = 50,  SQUARE_D4 = 51,
    SQUARE_E4 = 52,  SQUARE_F4 = 53,  SQUARE_G4 = 54,  SQUARE_H4 = 55,
    SQUARE_A5 = 64,  SQUARE_B5 = 65,  SQUARE_C5 = 66,  SQUARE_D5 = 67,
    SQUARE_E5 = 68,  SQUARE_F5 = 69,  SQUARE_G5 = 70,  SQUARE_H5 = 71,
    SQUARE_A6 = 80,  SQUARE_B6 = 81,  SQUARE_C6 = 82,  SQUARE_D6 = 83,
    SQUARE_E6 = 84,  SQUARE_F6 = 85,  SQUARE_G6 = 86,  SQUARE_H6 = 87,
    SQUARE_A7 = 96,  SQUARE_B7 = 97,  SQUARE_C7 = 98,  SQUARE_D7 = 99,
    SQUARE_E7 = 100, SQUARE_F7 = 101, SQUARE_G7 = 102, SQUARE_H7 = 103,
    SQUARE_A8 = 112, SQUARE_B8 = 113, SQUARE_C8 = 114, SQUARE_D8 = 115,
    SQUARE_E8 = 116, SQUARE_F8 = 117, SQUARE_G8 = 118, SQUARE_H8 = 119
};

constexpr int KNIGHT_DELTAS[8] = {-33, -31, -18, -14, 14, 18, 31, 33};
constexpr int BISHOP_DELTAS[4] = {-17, -15, 15, 17};
constexpr int ROOK_DELTAS[4]   = {-16, -1, 1, 16};
constexpr int QUEEN_DELTAS[8]  = {-17, -16, -15, -1, 1, 15, 16, 17};
constexpr int KING_DELTAS[8]   = {-17, -16, -15, -1, 1, 15, 16, 17};

constexpr int CENTER_SQUARES[4] = {SQUARE_D4, SQUARE_E4, SQUARE_D5, SQUARE_E5};
constexpr int EXTENDED_CENTER[16] = {
    SQUARE_C3, SQUARE_D3, SQUARE_E3, SQUARE_F3,
    SQUARE_C4, SQUARE_D4, SQUARE_E4, SQUARE_F4,
    SQUARE_C5, SQUARE_D5, SQUARE_E5, SQUARE_F5,
    SQUARE_C6, SQUARE_D6, SQUARE_E6, SQUARE_F6
};

constexpr int SEE_PIECE_VALUES[7] = {0, 100, 320, 330, 500, 950, 20000};
constexpr int PIECE_PHASE_VALUE[7] = {0, 0, 1, 1, 2, 4, 0};

struct Piece {
    uint8_t piece;
    uint8_t color;
};

struct Move {
    uint8_t from;
    uint8_t to;
    uint8_t piece;
    uint8_t captured;
    uint8_t promotion;
    uint8_t flags;
    int score;
};

struct UndoInfo {
    Move move;
    int castlingRights;
    int enPassantSquare;
    int halfMoveClock;
    uint8_t capturedPiece;
    uint8_t capturedColor;
    int sideToMove;
    bool isNull;
    int epCaptureSq;
    uint8_t epCapturePiece;
    uint8_t epCaptureColor;
    int castleRookFrom;
    int castleRookTo;
};

struct EngineOptions {
    int hashSizeMB = 64;
    int threads = 1;
    int skillLevel = 20;
    int contempt = 0;
    int nullMoveReduction = 3;
    int nullMoveDepthLimit = 3;
    int lmrDepthThreshold = 3;
    double lmrBaseReduction = 0.75;
    int lmrMoveCountThreshold = 3;
    int aspirationWindow = 25;
    int aspirationWindowMinDepth = 4;
    int futilityMargin = 100;
    int futilityDepthLimit = 3;
    int razorMargin = 300;
    int razorDepthLimit = 2;
    int singularExtensionMargin = 50;
    int singularExtensionDepth = 6;
    int kingSafetyWeight = 100;
    int mobilityWeight = 8;
    int pawnStructureWeight = 12;
    int passedPawnWeight = 50;
    int isolatedPawnPenalty = 15;
    int doubledPawnPenalty = 10;
    int backwardPawnPenalty = 12;
    int bishopPairBonus = 30;
    int rookOnOpenFile = 20;
    int rookOnSemiOpenFile = 10;
    int rookOnSeventhRank = 20;
    int knightOutpostBonus = 15;
    int knightOnRimPenalty = 5;
    int tempoBonus = 10;
    int spaceWeight = 2;
    int threatWeight = 5;
    int trappedPiecePenalty = 50;
    int overextendedPawnPenalty = 8;
    int pawnChainBonus = 8;
    int centralPawnBonus = 15;
    int advancedPawnBonus = 10;
    int kingTropismWeight = 3;
    int pawnShieldWeight = 20;
    int pawnStormWeight = 15;
    int attackZoneWeight = 10;
    int queenEarlyDevelopmentPenalty = 10;
    int rookCoordinationBonus = 5;
    int minorBehindPawnBonus = 5;
    int badBishopPenalty = 8;
    int pinnedPiecePenalty = 10;
    int discoveryThreatBonus = 15;
    int materialImbalanceWeight = 5;
    int initiativeWeight = 5;
    int drawScore = 0;
    bool verboseUCI = false;
    bool ponderEnabled = false;
    int multipv = 1;
    int slowMover = 100;
    int moveOverhead = 30;
    int minimumThinkingTime = 20;
    int emergencyTimeBuffer = 60;
    int slowMoverMin = 10;
    int slowMoverMax = 1000;
    int contemptDrawScore = 10;
    bool analyzeMode = false;
    bool showCurrentLine = false;
    std::string syzygyPath = "";
    int syzygyProbeDepth = 1;
    int syzygyProbeLimit = 6;
    bool cleanSearch = true;
    bool pruneAtRoot = false;
    int selectiveDepth = 64;
};

struct SearchStats {
    uint64_t nodes = 0;
    uint64_t qnodes = 0;
    uint64_t tthits = 0;
    uint64_t ttcuts = 0;
    uint64_t nullCuts = 0;
    uint64_t lmrReductions = 0;
    uint64_t futilityPrunes = 0;
    uint64_t razorPrunes = 0;
    uint64_t extensions = 0;
    uint64_t checkExtensions = 0;
    uint64_t singularExtensions = 0;
    uint64_t aspirationResearches = 0;
    uint64_t nps = 0;
};

struct SearchStackEntry {
    std::vector<Move> pv;
    int staticEval = 0;
    std::array<Move, 2> killers;
    Move currentMove;
    Move excludedMove;
    bool inCheck = false;
    int moveCount = 0;
};

struct TTEntry {
    uint64_t key = 0;
    int depth = 0;
    int score = 0;
    int flag = TT_EXACT;
    uint8_t moveFrom = 0;
    uint8_t moveTo = 0;
    uint8_t movePromotion = 0;
    int age = 0;
};

struct BookEntry {
    std::string move;
    int weight = 1;
};

struct PieceValues {
    int pawn = VALUE_PAWN;
    int knight = VALUE_KNIGHT;
    int bishop = VALUE_BISHOP;
    int rook = VALUE_ROOK;
    int queen = VALUE_QUEEN;
    int king = VALUE_KING;
};

struct ABResult {
    int score;
    Move move;
    std::vector<Move> pv;
};

struct AttackerInfo {
    int sq;
    int piece;
};

}

#endif
