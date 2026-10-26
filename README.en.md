[Polski](README.md) | **English**

# Tic-Tac-Toe with a MiniMax AI

A generalised tic-tac-toe game in C++ where you play as **X** against a computer opponent (**O**) driven by the **MiniMax algorithm with alpha-beta pruning**. The board size and the number of marks in a row needed to win are configurable.

Two versions are included:

- **`main.cpp`** — console version,
- **`main_sfml.cpp`** — graphical version built with **SFML 3** (mouse controls, +/− buttons for settings, score counter, AI move time and game time).

## Rules and settings

- Board size **n × n**, from 3 × 3 to 10 × 10 (the console version accepts any size).
- Number of marks in a row needed to win: from 3 up to `min(n, 6)`.
- A line counts horizontally, vertically and on both diagonals.
- You choose who moves first; the game keeps a running score of X wins, O wins and draws.

## How the AI works

1. If the AI can win in one move, it plays that move.
2. If the player could win on their next move, the AI blocks it.
3. Otherwise it runs MiniMax with alpha-beta pruning. The search depth depends on the board size (9 for 3 × 3, down to 3 for boards 8 × 8 and larger) so that each move takes a reasonable time.
4. When the depth limit is reached, the position is scored by a heuristic that rewards lines containing only one player's marks (more marks — higher score) and squares close to the centre.

The time the AI needed for each move is shown after it plays.

## Build and run

### Console version

```bash
g++ -std=c++17 -O2 main.cpp -o main
./main
```

### Graphical version (SFML 3)

```bash
g++ -std=c++17 -O2 main_sfml.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system
./game
```

`arial.ttf` must be in the working directory. On Windows the repository already contains the SFML 3 DLLs and a built `game.exe`.
