# Yuuki Chess Engine

**Version:** 1.1  
**Author:** [Eryx/RJ](https://facebook.com/profile.php?id=61590704998111)  
**License:** MIT  

Yuuki is a UCI-compatible chess engine written in C++17. It features an advanced evaluation function, sophisticated search algorithms, and a modular codebase designed for clarity and extensibility.

---

## 📦 Version

### Latest Release: v1.1

Get pre-compiled binaries for your platform:

- **[Android (ARM64-v8a)](https://github.com/eryxveilen/yuuki-chess-engine/releases)** – For DroidFish on Android
- **[Linux (x64)](https://github.com/eryxveilen/yuuki-chess-engine/releases)** – For Linux systems
- **[macOS (x64)](https://github.com/eryxveilen/yuuki-chess-engine/releases)** – For macOS
- **[Windows (x64)](https://github.com/eryxveilen/yuuki-chess-engine/releases)** – For Windows

### Web Snapshots

| Snapshot | Description | Link |
|----------|-------------|------|
| **Main** | Full preset personalities (Sniper, Super GM, Solid Defender, Pawn Master) | [Yuuki App on Base44](https://yuuki-ch.base44.app) |
| **v1.2 Snapshot** | Engine resigns logic update — improved human-like behavior | [Snapshot v1.2](https://yuuki-ch-snapv1-2.base44.app/) |

> ⚠️ *Known issues fixed thanks to [Anne](https://github.com/sarinaanneperez-dev)'s contribution via Base44 Builder+ plan.*

---

## 📦 Installation

### Android (arm64-v8a)

1. Download [yuuki-android-arm64-v8a](https://github.com/eryxveilen/yuuki-chess-engine/releases/download/v1.1/yuuki-android-arm64-v8a) and [DroidFish.apk](https://f-droid.org/en/packages/org.petero.droidfish/)
2. Install DroidFish
3. Move the yuuki-android-arm64-v8a to: `Droidfish/uci/`
4. Open DroidFish → Left Toolbar → **"Manage Chess Engines"** → **"Select Engines"** → **"Yuuki"**

---

## 🎛️ Engine Personalities

Yuuki supports 50+ configurable parameters for different play styles.

### Quick Setup via INI Files

1. Download preset INI from [`settings preset`](https://github.com/eryxveilen/yuuki-chess-engine/tree/main/settings%20preset) folder (e.g., `SUPERGM.ini`, `A8-sniper.ini`)
2. Move to `Droidfish/uci/` folder
3. Rename to `yuuki-android-arm64-v8a.ini`
4. Restart DroidFish

### Manual Configuration

1. Open DroidFish → **Manage Chess Engines** → **Set Options**
2. Adjust values according to INI files
3. Save and return to board

**Example:**
```ini
SkillLevel=20
Contempt=40
KingSafetyWeight=100
```

> 💡 **Tip:** Keep multiple renamed INI files (`Yuuki_SuperGM.ini`, `Yuuki_Sniper.ini`) and swap them as needed.

---

## ✨ Features

### Core Engine

- **UCI Protocol** – Full Universal Chess Interface support
- **Advanced Search** – Alpha-beta pruning with iterative deepening and Principal Variation Search (PVS)
- **Transposition Table** – Configurable hash table (1–4096 MB) with Zobrist hashing
- **Sophisticated Evaluation** – Tapered evaluation with game-phase interpolation
- **Search Optimizations** – LMR, null-move pruning, futility pruning, razoring, aspiration windows, check extensions, singular extensions
- **Quiescence Search** – Captures-only search with delta pruning and SEE
- **Skill Levels** – 0–20 adjustable handicap mode
- **Customizable Piece Values** – Fully adjustable piece valuations
- **Opening Book** – Polyglot-compatible opening book support
- **Move Ordering** – Hash moves, killer moves, history heuristic, counter-moves, SEE-based capture ordering
- **Endgame Evaluation** – Passed pawns, rook endgames, opposite bishops, pawn races
- **Threading** – Up to 512 configurable threads

### Evaluation Components

| Component | Features |
|-----------|----------|
| **Material** | Base piece values with adjustable weights |
| **Piece-Square Tables** | Tapered MG/EG tables for all pieces |
| **Mobility** | Piece-specific bonuses for knights, bishops, rooks, queens |
| **Pawn Structure** | Isolated, doubled, backward, passed, candidate, chained, central pawns |
| **King Safety** | Pawn shield, pawn storm, attack zone, king tropism, open files |
| **Rooks** | Open/semi-open files, 7th-rank bonuses, coordination |
| **Knights** | Outpost detection, rim penalties, pawn protection |
| **Bishops** | Bishop pair bonus, bad bishop penalty, discovery threats |
| **Threats** | Hanging pieces, queen development, trapped pieces |
| **Space** | Center and extended center control |
| **Tropism** | Piece proximity to enemy king |
| **Pins** | Pinned piece penalties |
| **Material Imbalance** | Minor-piece and rook/queen imbalances |
| **Initiative** | Mobility advantage, center control, tempo |
| **Tactical Motifs** | Forks, undefended pieces, threats |
| **Piece Coordination** | Minor coordination, rook alignment, queen-minor links |
| **Weaknesses** | Undefended pieces, weak pawns near king |
| **Endgame Specials** | Passed pawn races, opposite bishops, rook endgames |

---

## 🚀 Usage

### Running the Engine

```bash
./yuuki
```

### UCI Commands

```
uci
isready
ucinewgame
position startpos moves e2e4
go depth 12
```

### Console Commands

| Command | Description |
|---------|-------------|
| `uci` | Display engine info and options |
| `d` | Display current board position |
| `eval` | Show static evaluation score |
| `perft [depth]` | Run perft divide test |
| `divide [depth]` | Run perft divide with per-move counts |
| `go depth N` | Search to depth N |
| `go movetime MS` | Search for MS milliseconds |
| `go infinite` | Search until `stop` command |
| `stop` | Stop current search |
| `ponderhit` | Respond to pondering hit |
| `debug [on/off]` | Enable/disable debug output |

---

## ⚙️ UCI Options

| Option | Type | Default | Range |
|--------|------|---------|-------|
| Hash | spin | 64 | 1–4096 MB |
| Clear Hash | button | – | – |
| Ponder | check | false | – |
| MultiPV | spin | 1 | 1–500 |
| Skill Level | spin | 20 | 0–20 |
| Move Overhead | spin | 30 | 0–5000 ms |
| Minimum Thinking Time | spin | 20 | 0–5000 ms |
| Slow Mover | spin | 100 | 10–1000 |
| Threads | spin | 1 | 1–512 |
| UCI_AnalyseMode | check | false | – |
| UCI_ShowCurrLine | check | false | – |
| Contempt | spin | 0 | -100–100 |
| DrawScore | spin | 0 | -100–100 |
| PawnValue | spin | 100 | 0–2000 |
| KnightValue | spin | 320 | 0–2000 |
| BishopValue | spin | 330 | 0–2000 |
| RookValue | spin | 500 | 0–2000 |
| QueenValue | spin | 950 | 0–4000 |
| KingValue | spin | 20000 | 0–100000 |
| NullMoveReduction | spin | 3 | 1–10 |
| NullMoveDepthLimit | spin | 3 | 1–20 |
| LMRDepthThreshold | spin | 3 | 1–20 |
| LMRBaseReduction | spin | 75 | 0–200 |
| LMRMoveCountThreshold | spin | 3 | 1–20 |
| AspirationWindow | spin | 25 | 1–500 |
| AspirationWindowMinDepth | spin | 4 | 1–20 |
| FutilityMargin | spin | 100 | 0–1000 |
| FutilityDepthLimit | spin | 3 | 0–10 |
| RazorMargin | spin | 300 | 0–1000 |
| RazorDepthLimit | spin | 2 | 0–10 |
| SingularExtensionMargin | spin | 50 | 0–500 |
| SingularExtensionDepth | spin | 6 | 1–20 |
| KingSafetyWeight | spin | 100 | 0–500 |
| MobilityWeight | spin | 8 | 0–100 |
| PawnStructureWeight | spin | 12 | 0–200 |
| PassedPawnWeight | spin | 50 | 0–200 |
| IsolatedPawnPenalty | spin | 15 | 0–100 |
| DoubledPawnPenalty | spin | 10 | 0–100 |
| BackwardPawnPenalty | spin | 12 | 0–100 |
| BishopPairBonus | spin | 30 | 0–100 |
| RookOnOpenFile | spin | 20 | 0–100 |
| RookOnSemiOpenFile | spin | 10 | 0–100 |
| RookOnSeventhRank | spin | 20 | 0–100 |
| KnightOutpostBonus | spin | 15 | 0–100 |
| KnightOnRimPenalty | spin | 5 | 0–50 |
| TempoBonus | spin | 10 | 0–100 |
| SpaceWeight | spin | 2 | 0–50 |
| ThreatWeight | spin | 5 | 0–50 |
| TrappedPiecePenalty | spin | 50 | 0–200 |
| OverextendedPawnPenalty | spin | 8 | 0–50 |
| PawnChainBonus | spin | 8 | 0–50 |
| CentralPawnBonus | spin | 15 | 0–100 |
| AdvancedPawnBonus | spin | 10 | 0–100 |
| KingTropismWeight | spin | 3 | 0–30 |
| PawnShieldWeight | spin | 20 | 0–100 |
| PawnStormWeight | spin | 15 | 0–100 |
| AttackZoneWeight | spin | 10 | 0–100 |
| QueenEarlyDevelopmentPenalty | spin | 10 | 0–100 |
| RookCoordinationBonus | spin | 5 | 0–50 |
| MinorBehindPawnBonus | spin | 5 | 0–50 |
| BadBishopPenalty | spin | 8 | 0–50 |
| PinnedPiecePenalty | spin | 10 | 0–100 |
| DiscoveryThreatBonus | spin | 15 | 0–100 |
| MaterialImbalanceWeight | spin | 5 | 0–50 |
| InitiativeWeight | spin | 5 | 0–50 |
| VerboseUCI | check | false | – |
| OwnBook | check | false | – |
| Clean Search | check | true | – |
| SelectiveDepth | spin | 64 | 1–128 |

---

## 🏗️ Architecture

### Board Representation (0x88)

Yuuki uses a 128-square 0x88 board representation for efficient move validation.

**Square Indexing:**
```cpp
int sq = (rank << 4) | file;
bool isValid = (sq & 0x88) == 0;
```

**Piece Definition:**
```cpp
struct Piece {
    uint8_t piece;
    uint8_t color;
};
```

**Constants:**
```cpp
namespace Yuuki {
    const int PIECE_NONE = 0;
    const int PIECE_PAWN = 1;
    const int PIECE_KNIGHT = 2;
    const int PIECE_BISHOP = 3;
    const int PIECE_ROOK = 4;
    const int PIECE_QUEEN = 5;
    const int PIECE_KING = 6;

    const int COLOR_WHITE = 0;
    const int COLOR_BLACK = 1;

    const int CASTLE_WK = 1;
    const int CASTLE_WQ = 2;
    const int CASTLE_BK = 4;
    const int CASTLE_BQ = 8;
}
```

### Move Representation

```cpp
struct Move {
    int from;
    int to;
    int piece;
    int captured;
    int promotion;
    int flags;
    int score;
};

const int FLAG_NONE = 0;
const int FLAG_ENPASSANT = 1;
const int FLAG_CASTLING = 2;
const int FLAG_PAWN_DOUBLE = 4;
const int FLAG_PROMOTION = 8;
```

### Zobrist Hashing

```cpp
uint64_t g_zobristKeys[128][2][7];
uint64_t g_zobristSide;
uint64_t g_zobristCastling[4];
uint64_t g_zobristEnPassant[128];

uint64_t getPositionHash() {
    return computeZobristHash();
}
```

### Game Phases

Tapered evaluation smoothly interpolates between middlegame and endgame:

```cpp
void updateGamePhase() {
    int phase = 0;
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) {
            const Piece& p = g_board[i];
            if (p.piece != PIECE_NONE && p.piece != PIECE_PAWN && p.piece != PIECE_KING) {
                phase += PIECE_PHASE_VALUE[p.piece];
            }
        }
    }
    g_phaseScore = phase;
    if (phase > 18) {
        g_gamePhase = GAME_PHASE_OPENING;
    } else if (phase > 8) {
        g_gamePhase = GAME_PHASE_MIDDLEGAME;
    } else {
        g_gamePhase = GAME_PHASE_ENDGAME;
    }
}

int interpolateScore(int mgScore, int egScore) {
    int phase = g_phaseScore;
    if (phase > 24) phase = 24;
    return ((mgScore * phase) + (egScore * (24 - phase))) / 24;
}
```

**Phase Values:**
- Knight/Bishop: 1 point each
- Rook: 2 points each
- Queen: 4 points each
- Pawns/Kings: 0 points
- Total max: 24

### Move Generation

Pseudo-legal move generation validated via make/undo:

```cpp
std::vector<Move> generateLegalMovesImpl(int color) {
    auto pseudoMoves = generatePseudoLegalMoves(color, false);
    std::vector<Move> legalMoves;
    legalMoves.reserve(pseudoMoves.size());

    for (const auto& move : pseudoMoves) {
        makeMove(move);
        if (!isInCheck(color)) {
            legalMoves.push_back(move);
        }
        undoMove();
    }

    return legalMoves;
}
```

### Search Algorithm

**Iterative Deepening with Aspiration Windows:**

```cpp
Move iterativeDeepening(int maxDepth, int timeLimit, bool infinite) {
    Move bestMove = {0, 0, 0, 0, 0, 0, 0};
    int bestScore = 0;
    int alpha = -SCORE_INFINITE;
    int beta = SCORE_INFINITE;
    
    for (int depth = 1; depth <= maxDepth; depth++) {
        if (shouldStopTime(timeLimit)) break;
        
        int window = g_engineOptions.aspirationWindow;
        alpha = bestScore - window;
        beta = bestScore + window;
        
        ABResult result = alphaBetaRoot(depth, alpha, beta);
        
        if (result.score <= alpha || result.score >= beta) {
            result = alphaBetaRoot(depth, -SCORE_INFINITE, SCORE_INFINITE);
        }
        
        bestScore = result.score;
        bestMove = result.move;
    }
    
    return bestMove;
}
```

**Principal Variation Search:**

```cpp
ABResult alphaBeta(int ply, int depth, int alpha, int beta, bool isPV, bool allowNull) {
    if (depth <= 0) {
        return {quiescence(ply, alpha, beta, 0), Move{0, 0, 0, 0, 0, 0, 0}};
    }

    uint64_t hash = getPositionHash();
    TTEntry* ttEntry = probeTranspositionTable(hash, depth, alpha, beta);
    if (ttEntry != nullptr) {
        return {ttEntry->score, Move{ttEntry->moveFrom, ttEntry->moveTo, 0, 0, ttEntry->movePromotion, 0, 0}};
    }

    int staticEval = evaluate();
    
    if (isInCheck(g_sideToMove)) {
        depth++;
    }

    if (allowNull && staticEval >= beta && depth >= g_engineOptions.nullMoveDepthLimit) {
        makeNullMove();
        ABResult result = alphaBeta(ply + 1, depth - g_engineOptions.nullMoveReduction - 1, -beta, -beta + 1, false, false);
        undoNullMove();
        
        if (-result.score >= beta) {
            return {beta, Move{0, 0, 0, 0, 0, 0, 0}};
        }
    }

    std::vector<Move> moves = generatePseudoLegalMoves(g_sideToMove, false);
    orderMoves(moves, hash, ply);

    int alphaOrig = alpha;
    ABResult best = {alpha, Move{0, 0, 0, 0, 0, 0, 0}};
    int moveCount = 0;

    for (const auto& move : moves) {
        makeMove(move);
        if (isInCheck(1 - g_sideToMove)) {
            undoMove();
            continue;
        }

        ABResult result;
        if (moveCount == 0) {
            result = alphaBeta(ply + 1, depth - 1, -beta, -alpha, isPV, true);
            result.score = -result.score;
        } else {
            result = alphaBeta(ply + 1, depth - 1, -alpha - 1, -alpha, false, true);
            result.score = -result.score;
            
            if (result.score > alpha && result.score < beta) {
                result = alphaBeta(ply + 1, depth - 1, -beta, -result.score, isPV, true);
                result.score = -result.score;
            }
        }

        undoMove();
        moveCount++;

        if (result.score > best.score) {
            best.score = result.score;
            best.move = move;
            
            if (best.score > alpha) {
                alpha = best.score;
                if (alpha >= beta) {
                    storeTranspositionTable(hash, depth, alpha, TT_BETA, move);
                    return best;
                }
            }
        }
    }

    int flag = (best.score <= alphaOrig) ? TT_ALPHA : (best.score >= beta) ? TT_BETA : TT_EXACT;
    storeTranspositionTable(hash, depth, best.score, flag, best.move);
    
    return best;
}
```

**Quiescence Search:**

```cpp
int quiescence(int ply, int alpha, int beta, int qDepth) {
    int standPat = evaluate();
    
    if (standPat >= beta) return beta;
    if (standPat > alpha) alpha = standPat;

    const int DELTA_MARGIN = 200;
    if (standPat + DELTA_MARGIN < alpha) return alpha;

    std::vector<Move> captures = generateCaptureMoves(g_sideToMove);
    orderMoves(captures, 0, ply);

    for (const auto& move : captures) {
        makeMove(move);
        
        int score = -quiescence(ply + 1, -beta, -alpha, qDepth + 1);
        
        undoMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}
```

### Move Ordering

Move ordering uses a multi-stage scoring system to prioritize moves:

```cpp
void scoreMoves(std::vector<Move>& moves, uint64_t hash, int ply) {
    for (auto& move : moves) {
        move.score = 0;

        if (move.captured != PIECE_NONE) {
            int seeVal = SEE_PIECE_VALUES[move.captured] * 10 - SEE_PIECE_VALUES[move.piece];
            move.score += 100000 + seeVal;
        }

        if (move.promotion != PIECE_NONE) {
            move.score += 90000 + getPieceValue(move.promotion);
        }

        if (g_ttSize > 0) {
            size_t idx = hash % g_ttSize;
            TTEntry* entry = &g_transpositionTable[idx];
            if (entry->key == hash && entry->moveFrom == move.from && entry->moveTo == move.to) {
                move.score += 200000;
            }
        }

        if (ply < MAX_PLY) {
            if (movesEqual(move, g_killerMoves[ply][0])) move.score += 80000;
            if (movesEqual(move, g_killerMoves[ply][1])) move.score += 70000;
        }

        int historyScore = g_historyTable[g_sideToMove][move.from][move.to];
        if (historyScore) {
            move.score += std::min(historyScore, 60000);
        }

        if (!g_moveHistory.empty()) {
            const Move& lastMove = g_moveHistory.back().move;
            if (g_counterMovesValid[1 - g_sideToMove][lastMove.from][lastMove.to]) {
                Move counterMove = g_counterMoves[1 - g_sideToMove][lastMove.from][lastMove.to];
                if (movesEqual(move, counterMove)) {
                    move.score += 65000;
                }
            }
        }
    }
}

void orderMoves(std::vector<Move>& moves, uint64_t hash, int ply) {
    scoreMoves(moves, hash, ply);
    
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}
```

**Priority order (by move score):**
1. Hash move: +200000
2. Winning captures (SEE > 0): +100000 + SEE value
3. Promotions: +90000 + promotion piece value
4. Killer moves (1st): +80000
5. Killer moves (2nd): +70000
6. Counter-moves: +65000
7. History heuristic: +min(score, 60000)
8. Other moves: 0

### Evaluation Example

```cpp
int evaluateMaterial() {
    int score = 0;
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) {
            const Piece& p = g_board[i];
            if (p.piece != PIECE_NONE) {
                int value = getPieceValue(p.piece);
                score += (p.color == COLOR_WHITE) ? value : -value;
            }
        }
    }
    return score;
}

int evaluateFull() {
    int mgScore = 0;
    int egScore = 0;

    mgScore += evaluateMaterial();
    mgScore += evaluatePST();
    mgScore += evaluateMobility();
    mgScore += evaluatePawnStructure();
    mgScore += evaluateKingSafety();
    mgScore += evaluateRooks();
    mgScore += evaluateKnights();
    mgScore += evaluateBishops();
    mgScore += evaluateThreats();
    mgScore += evaluateSpace();

    egScore += evaluateMaterial();
    egScore += evaluatePST();
    egScore += evaluateMobility();
    egScore += evaluatePawnStructure();
    egScore += evaluateRooks();

    int finalScore = interpolateScore(mgScore, egScore);
    
    if (g_sideToMove == COLOR_BLACK) {
        finalScore = -finalScore;
    }

    return finalScore;
}
```

### Static Exchange Evaluation (SEE)

```cpp
int see(int sq) {
    int seeGain[32];
    Piece occupiers[128];
    for (int i = 0; i < 128; i++) {
        if (isValidSquare(i)) occupiers[i] = g_board[i];
        else occupiers[i] = {PIECE_NONE, 0};
    }

    int targetPiece = occupiers[sq].piece;
    int depth = 0;
    int currentColor = g_sideToMove;

    seeGain[depth] = targetPiece != PIECE_NONE ? SEE_PIECE_VALUES[targetPiece] : 0;

    do {
        std::vector<std::pair<int, int>> attackers;
        for (int i = 0; i < 128; i++) {
            if (!isValidSquare(i) || occupiers[i].piece == PIECE_NONE || occupiers[i].color != currentColor) continue;
            const Piece& piece = occupiers[i];
            
            if (pieceAttacksSquare(i, piece, sq)) {
                attackers.push_back({SEE_PIECE_VALUES[piece.piece], i});
            }
        }

        if (attackers.empty()) break;

        std::sort(attackers.begin(), attackers.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        auto attacker = attackers[0];
        depth++;
        seeGain[depth] = SEE_PIECE_VALUES[targetPiece] - seeGain[depth - 1];
        targetPiece = occupiers[attacker.second].piece;
        occupiers[attacker.second] = {PIECE_NONE, 0};
        currentColor = 1 - currentColor;

    } while (depth < 31);

    while (depth > 0) {
        seeGain[depth - 1] = -std::max(-seeGain[depth - 1], seeGain[depth]);
        depth--;
    }

    return seeGain[0];
}
```

### Transposition Table

```cpp
struct TTEntry {
    uint64_t key;
    int depth;
    int score;
    int flag;
    int moveFrom;
    int moveTo;
    int movePromotion;
    int age;
};

const int TT_EXACT = 0;
const int TT_ALPHA = 1;
const int TT_BETA = 2;

void storeTranspositionTable(uint64_t hash, int depth, int score, int flag, const Move& move) {
    if (g_ttSize == 0) return;
    size_t idx = hash % g_ttSize;
    TTEntry* entry = &g_transpositionTable[idx];
    if (entry->key != 0 && entry->depth > depth + 2) return;
    entry->key = hash;
    entry->depth = depth;
    entry->score = score;
    entry->flag = flag;
    entry->moveFrom = move.from;
    entry->moveTo = move.to;
    entry->movePromotion = move.promotion;
    entry->age = g_fullMoveNumber;
}
```

### Piece-Square Tables

Example pawn PST (tapered):

```
Middlegame:                     Endgame:
0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
50  50  50  50  50  50  50  50  80  80  80  80  80  80  80  80
10  10  20  30  30  20  10  10  50  50  50  50  50  50  50  50
5   5   10  25  25  10  5   5   30  30  30  30  30  30  30  30
0   0   0   20  20  0   0   0   20  20  20  20  20  20  20  20
5   -5  -10 0   0   -10 -5  5   10  10  10  10  10  10  10  10
5   10  10  -20 -20 10  10  5   0   0   0   0   0   0   0   0
0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
```

---

## 📊 Performance

- **Transposition Table**: Up to 4096 MB with configurable sizing
- **LMR Table**: Precomputed reductions up to depth 64
- **Zobrist Hashing**: 64-bit keys for fast position identification
- **Move Generation**: Optimized 0x88 board representation
- **Time Management**: Dynamic allocation with move overhead compensation
- **Skill Levels**: Proportional node reduction + intentional tactical oversights

---

## 📝 Version History

- **1.0** – Initial release with full UCI support
- **1.1** – Improved engine strength, optimized memory usage

---

## 🙏 Credits

- **Engine Developer**: [Eryx/RJ](https://facebook.com/profile.php?id=61590704998111)
- **Web Interface & Testing**: [Anne](https://github.com/sarinaanneperez-dev) – Interface tweaks and deployment support via Base44 Builder+ plan.
- **Testing Platform**: [Base44.app](https://base44.app) – Used for web deployment and engine testing.

---

## 📄 License

MIT License – see [LICENSE](LICENSE) for details.
