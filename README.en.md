🇵🇱 [Polski](README.md) | 🇬🇧 **English**

# ❌⭕ Tic-Tac-Toe with a MiniMax AI

![C++](https://img.shields.io/badge/C++-00599C?logo=cplusplus&logoColor=white) ![SFML](https://img.shields.io/badge/SFML_3-8CC445?logo=sfml&logoColor=white)

> 🎓 Project for the **Algorithm Design and Analysis** course (Wrocław University of Science and Technology)

Generalised tic-tac-toe with a computer opponent based on **MiniMax with alpha-beta pruning**. Boards from 3×3 to 10×10, with 3 to 6 marks in a row needed to win.

## ✨ What I did

- **AI** — first checks for a winning move or a move it must block, then searches the game tree with MiniMax and alpha-beta pruning
- **Search depth adapted to the board** — 9 moves ahead on 3×3, fewer on bigger boards, so the AI stays fast
- **Evaluation heuristic** — counts the lines each player can still win and rewards the centre of the board
- Two versions: **console** and **graphical in SFML 3** (mouse controls, score counter, AI move time)

## 🛠️ Tech

C++17 · SFML 3

## 🚀 Run it

```bash
# console version
g++ -std=c++17 -O2 main.cpp -o main && ./main

# graphical version (needs SFML 3)
g++ -std=c++17 -O2 main_sfml.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system && ./game
```

On Windows, a prebuilt `game.exe` with the SFML libraries is included.
