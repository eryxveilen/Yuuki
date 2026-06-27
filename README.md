# Yuuki Chess Engine

**Version:** 1.0  
**Author:** [Eryx/RJ](https://facebook.com/profile.php?id=61590704998111)    
**License:** MIT  

Yuuki is a UCI-compatible chess engine written in JavaScript (Web Worker) and C++17. It features an advanced evaluation function, sophisticated search algorithms, and a modular codebase designed for clarity and extensibility. There's a release here compatible with arm64-v8a. Feel free to compile it to be compatible with your devices!

---

## Installation (for arm64-v8a)

1. Extract the [Yuuki.zip](https://github.com/user-attachments/files/29368204/Yuuki.zip) then load it to Chess GUI Apps.
2. Recommended App: [DroidFish.apk](https://f-droid.org/en/packages/org.petero.droidfish/)

### Step-by-step:

1. Download [Yuuki.zip](https://github.com/user-attachments/files/29368204/Yuuki.zip) and [DroidFish.apk](https://f-droid.org/en/packages/org.petero.droidfish/)
2. Install Droidfish and Extract Yuuki.zip
3. Move the extracted file to: `Droidfish/uci/`
4. Open Droidfish, open the left toolbar.
5. Click **"Manage Chess Engines"** > **"Select Engines"** > **"Yuuki"**

You can tweak its gameplay with 50+ controls on engine playstyle, making it very unique among any other engines:  
**"Manage Chess Engines"** > **"Set Options"**

---
### ALTERNATIVELY, YOU CAN USE THIS ENGINE ON [Yuuki App on Base44.com](https://yuuki-ch.base44.app)
**Notice:** There's a bug in this app because it's not handled properly by the code in the web, causing it to generate illegal move (but very rare case).
---

## Features

### Core Engine

- **UCI Protocol** – Full support for Universal Chess Interface.
- **Advanced Search** – Alpha-beta pruning with iterative deepening, Principal Variation Search (PVS).
- **Transposition Table** – Configurable hash table (up to 4096 MB) for move caching with Zobrist hashing.
- **Sophisticated Evaluation** – Tapered evaluation with game-phase interpolation (opening/middlegame/endgame).
- **Search Optimizations** – Late Move Reductions (LMR), null-move pruning, futility pruning, razoring, aspiration windows, check extensions, singular extensions.
- **Quiescence Search** – Captures-only search with delta pruning and SEE (Static Exchange Evaluation).
- **Skill Levels** – Adjustable from 0–20 for handicapped play.
- **Piece Values** – Fully adjustable piece values (pawn, knight, bishop, rook, queen, king).
- **Opening Book** – Polyglot-compatible opening book support.
- **Move Ordering** – Killer moves, history heuristic, counter-moves, and hash-move ordering.
- **Endgame Evaluation** – Specialized endgame scoring: passed pawns, rook endgames, opposite bishops, pawn races.
- **Threading** – Supports up to 512 threads (configurable).

### Evaluation Components

| Component | Description |
|-----------|-------------|
| **Material** | Base piece values with adjustable weights. |
| **Piece-Square Tables (PST)** | Tapered MG/EG tables for all pieces. |
| **Mobility** | Piece-specific mobility bonuses for knights, bishops, rooks, queens. |
| **Pawn Structure** | Isolated, doubled, backward, passed, candidate, chained, central, advanced pawns. |
| **King Safety** | Pawn shield, pawn storm, attack zone, king tropism, open files near king. |
| **Rooks** | Open/semi-open files, 7th-rank bonuses, rook coordination. |
| **Knights** | Outpost detection, rim penalties, pawn protection. |
| **Bishops** | Bishop pair bonus, bad bishop penalty, mobility traps, discovery threats. |
| **Threats** | Hanging pieces, queen early development, trapped pieces. |
| **Space** | Control of key squares in the center and extended center. |
| **Tropism** | Piece proximity to enemy king. |
| **Pins** | Penalty for pinned pieces. |
| **Material Imbalance** | Minor-piece imbalances, rook/queen imbalances. |
| **Initiative** | Mobility advantage, center control, tempo bonus. |
| **Tactical Motifs** | Forks, undefended pieces, tactical threats. |
| **Piece Coordination** | Minor-piece coordination, rook alignment, queen-minor links. |
| **Weaknesses** | Undefended pieces, weak pawns near king. |
| **Endgame Specials** | Passed pawn races, opposite bishops, rook endgames. |

---

## Usage

### As UCI Engine

```bash
./yuuki
```

Then use standard UCI commands:

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
| `uci` | Display engine info and options. |
| `d` | Display current board position. |
| `eval` | Show static evaluation score. |
| `perft [depth]` | Run perft divide test. |
| `divide [depth]` | Run perft divide and show per-move counts. |
| `go depth N` | Search to depth N. |
| `go movetime MS` | Search for MS milliseconds. |
| `go infinite` | Search until `stop` command. |
| `stop` | Stop current search. |
| `ponderhit` | Respond to pondering hit. |
| `debug [on/off]` | Enable/disable debug output. |

---

## UCI Options

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

## Architecture

### Board Representation (0x88)

Yuuki uses a 128-square 0x88 board representation:

- Squares are indexed as `(rank << 4) | file`
- Valid squares: `(sq & 0x88) === 0`
- Piece storage: `{ piece: PIECE_*, color: COLOR_* }`

**Piece Constants:**
```
PIECE_NONE = 0
PIECE_PAWN = 1
PIECE_KNIGHT = 2
PIECE_BISHOP = 3
PIECE_ROOK = 4
PIECE_QUEEN = 5
PIECE_KING = 6
```

**Color Constants:**
```
COLOR_WHITE = 0
COLOR_BLACK = 1
```

**Castling Rights:**
```
CASTLE_WK = 1  // White kingside
CASTLE_WQ = 2  // White queenside
CASTLE_BK = 4  // Black kingside
CASTLE_BQ = 8  // Black queenside
```

### Move Representation

```javascript
{
    from: number,          // Source square
    to: number,            // Destination square
    piece: number,         // Moving piece
    captured: number,      // Captured piece (or NONE)
    promotion: number,     // Promotion piece (or NONE)
    flags: number,         // FLAG_NONE, FLAG_ENPASSANT, FLAG_CASTLING, FLAG_PAWN_DOUBLE, FLAG_PROMOTION
    score: number          // Move ordering score
}
```

### Zobrist Hashing

- 64-bit keys for each piece on each square
- Separate keys for side-to-move, castling rights, and en-passant squares
- Used for:
  - Transposition table indexing
  - Position repetition detection
  - Pawn structure hashing (evaluation cache)

### Game Phases

Yuuki uses a **tapered evaluation** that smoothly interpolates between middlegame and endgame scores:

```
Phase score = sum of piece-phase values:
- Knight/Bishop: 1 point each
- Rook: 2 points each
- Queen: 4 points each
- Pawns/Kings: 0 points

Total phase max = 24 (2 knights + 2 bishops + 2 rooks + 1 queen)

Interpolation:
- phase > 18: Opening
- phase 8–18: Middlegame
- phase < 8: Endgame

Score = (mg_score * phase + eg_score * (24 - phase)) / 24
```

### Move Generation

Pseudo-legal move generation with legal validation via **make/undo**:

1. Generate all pseudo-legal moves for the current side.
2. For each move, `makeMove()` updates the board state.
3. Check if the current side's king is in check (`isInCheck()`).
4. If not in check, the move is legal.
5. `undoMove()` restores the previous state.

**Move Types:**
- Normal moves
- Pawn double pushes (sets en-passant flag)
- En-passant captures
- Castling (king-side and queen-side)
- Promotions (queen, rook, bishop, knight)

### Search Algorithm

**Principal Variation Search (PVS)** with iterative deepening:

```javascript
function iterativeDeepening(maxDepth, timeLimit, infinite) {
    for (depth = 1; depth <= maxDepth; depth++) {
        // Aspiration window
        alpha = bestScore - window
        beta = bestScore + window
        
        result = alphaBetaRoot(depth, alpha, beta)
        
        // Re-search on fail-low/fail-high
        if (score <= alpha || score >= beta) {
            result = alphaBetaRoot(depth, -INF, +INF)
        }
        
        // Time management
        if (stableMove && timeUsage > threshold) break
    }
}
```

**Alpha-Beta with LMR:**

```
alphaBeta(ply, depth, alpha, beta, isPV, allowNull) {
    // Transposition table probe
    if (ttResult.hit) return ttResult
    
    // Check extensions
    if (inCheck) depth++
    
    // Null-move pruning
    if (allowNull && staticEval >= beta && depth >= nullDepthLimit) {
        makeNullMove()
        score = -alphaBeta(..., beta-1)
        if (score >= beta) return beta
    }
    
    // Razoring
    if (depth <= razorDepth && staticEval + razorMargin < beta) {
        qScore = quiescence(...)
        if (qScore < beta) return qScore
    }
    
    // Futility pruning
    if (depth <= futilityDepth && staticEval - futilityMargin >= beta) {
        return beta
    }
    
    // Move ordering
    orderMoves(moves, hash, ply)
    
    for each move:
        makeMove()
        
        // Late Move Reduction
        if (depth >= LMRThreshold && moveCount >= LMRMoveThreshold) {
            reduction = LMR_Table[depth][moveCount]
            score = -alphaBeta(depth - reduction, alpha+1)
            if (score > alpha) {
                // Full-depth search
                score = -alphaBeta(depth - 1, ...)
            }
        }
        
        undoMove()
        updateKillers(), updateHistory()
        
        if (score > alpha) {
            alpha = score
            if (score >= beta) return beta
        }
}
```

### Quiescence Search

To avoid the **horizon effect**, Yuuki performs a quiescence search:

```
quiescence(ply, alpha, beta, qDepth) {
    standPat = evaluate()
    if (standPat >= beta) return beta
    if (standPat > alpha) alpha = standPat
    
    // Delta pruning
    if (standPat + delta < alpha) return alpha
    
    // Generate and sort captures
    captures = generateCaptures()
    for each capture:
        makeMove()
        score = -quiescence(...)
        undoMove()
        if (score >= beta) return beta
        if (score > alpha) alpha = score
    
    return alpha
}
```

### Move Ordering

Moves are scored and sorted using multiple heuristics:

| Priority | Source |
|----------|--------|
| 1 | Hash move (from transposition table) |
| 2 | Winning captures (SEE > 0) |
| 3 | Killer moves (two per ply) |
| 4 | Counter-moves |
| 5 | Equal captures (SEE = 0) |
| 6 | History heuristic |
| 7 | Losing captures (SEE < 0) |
| 8 | Remaining quiet moves |

### Evaluation Details

#### Material
Base values:
- Pawn: 100
- Knight: 320
- Bishop: 330
- Rook: 500
- Queen: 950
- King: 20000

#### Piece-Square Tables (PST)
Tapered tables for all pieces with **mirrored** values for black pieces. Example pawn PST:

```
Opening:                        Endgame:
0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
50  50  50  50  50  50  50  50  80  80  80  80  80  80  80  80
10  10  20  30  30  20  10  10  50  50  50  50  50  50  50  50
5   5   10  25  25  10  5   5   30  30  30  30  30  30  30  30
0   0   0   20  20  0   0   0   20  20  20  20  20  20  20  20
5   -5  -10 0   0   -10 -5  5   10  10  10  10  10  10  10  10
5   10  10  -20 -20 10  10  5   0   0   0   0   0   0   0   0
0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0
```

#### King Safety
- Pawn shield around king (weighted by proximity)
- Pawn storm from enemy pawns
- Attack zone: pieces within 3 squares of enemy king
- Open files near king
- Weak squares near king

#### Passed Pawns
- Bonus based on distance to promotion
- Connected passed pawns get additional bonus
- Candidate passed pawns (one enemy pawn blocking) get minor bonus

---

## Performance Notes

- **Transposition Table**: Size configurable up to 4096 MB, stores entries with depth, score, flag, and best move.
- **LMR Table**: Precomputed reduction table up to depth 64.
- **Futility/Razor Margins**: Dynamic margins based on depth.
- **History Heuristics**: Decay to prevent overflow.
- **Time Management**: Uses "Slow Mover" factor, move overhead, and emergency buffer.
- **Skill Levels**: Lower skill = fewer nodes searched + intentional blunders.

---

## Version History

- **1.0** – Initial release with full UCI support.

---

## Credits

- **Engine**: Eryx
- **Opening Book Format**: Polyglot compatible
- **Inspired By**: Stockfish, Ethereal, and other open-source chess engines

---

## License

MIT License – see [LICENSE](LICENSE) for details.
