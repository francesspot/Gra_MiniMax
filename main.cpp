#include <iostream>
#include <vector>
#include <chrono>
#include <cctype>

using namespace std;

int n; 
int toWin; 
vector<vector<char>> board; 

void initBoard(int n) {
  board.assign(n, vector<char>(n, '.')); 
}

void printBoard() {
  cout << "\n  ";
  for (int j = 0; j < n; j++) {
    cout << j + 1 << " ";
  }
  cout << "\n";

  for (int i = 0; i < n; i++) {
    cout << i + 1 << " ";
    for (int j = 0; j < n; j++) {
      cout << board[i][j] << " ";
    }
    cout << "\n";
  }
}

bool makeMove(int row, int col, char player) {
  if (row < 0 || row >= n || col < 0 || col >= n) {
    return false;
  }
  if (board[row][col] != '.') {
    return false;
  }
  board[row][col] = player;
  return true;
}

bool checkWin(int row, int col, char player) {
  int directions[4][2] = {
    {0, 1},   // Poziom
    {1, 0},   // Pion
    {1, 1},   // Przekątna "\"
    {1, -1}   // Przekątna "/"
  };

  for (int d = 0; d < 4; d++) {
    int dr = directions[d][0];
    int dc = directions[d][1];
    int count = 1;

    // Zliczanie w kierunku dodatnim
    int i = row + dr;
    int j = col + dc;
    while (i >= 0 && i < n && j >= 0 && j < n && board[i][j] == player) {
      count++;
      i += dr;
      j += dc;
    }

    // Zliczanie w kierunku ujemnym
    i = row - dr;
    j = col - dc;
    while (i >= 0 && i < n && j >= 0 && j < n && board[i][j] == player) {
      count++;
      i -= dr;
      j -= dc;
    }

    if (count >= toWin) {
      return true;
    }
  }
  return false;
}

bool isDraw() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (board[i][j] == '.') {
        return false;
      }
    }
  }
  return true;
}

// Zwraca listę dostępnych ruchów. Dla większych plansz skupia się na polach blisko już zajętych.
vector<pair<int, int>> getAvailableMoves() {
  vector<pair<int, int>> moves;

  if (n <= 7) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (board[i][j] == '.') {
          moves.push_back({i, j});
        }
      }
    }
    return moves;
  }

  vector<vector<bool>> used(n, vector<bool>(n, false));
  int radius = 2;

  for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
      if (board[r][c] == '.') {
        continue;
      }
      
      for (int dr = -radius; dr <= radius; dr++) {
        for (int dc = -radius; dc <= radius; dc++) {
          int nr = r + dr;
          int nc = c + dc;
          if (nr >= 0 && nr < n && nc >= 0 && nc < n && board[nr][nc] == '.' && !used[nr][nc]) {
            moves.push_back({nr, nc});
            used[nr][nc] = true;
          }
        }
      }
    }
  }

  // Zabezpieczenie na wypadek braku optymalnych ruchów - szukanie jakiegokolwiek wolnego pola
  if (moves.empty()) {
    if (board[n / 2][n / 2] == '.') {
      moves.push_back({n / 2, n / 2});
    } else {
      for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
          if (board[i][j] == '.') {
            moves.push_back({i, j});
            return moves;
          }
        }
      }
    }
  }
  return moves;
}

int evaluateLine(int countO, int countX) {
  if (countO == toWin - 1 && countX == 0) {
    return 5000; // Jeśli AI ma prawie wygraną linię, a gracz nie ma żadnych znaków w tej linii
  }
  if (countX == toWin - 1 && countO == 0) {
    return -5000; // Jeśli gracz ma prawie wygraną linię, a AI nie ma żadnych znaków w tej linii
  }
  if (countO > 0 && countX > 0) {
    return 0; // Jeśli obie strony mają znaki w tej linii, nie jest ona wartościowa dla żadnej ze stron
  }
  if (countO > 0 && countX == 0) {
    return countO * countO * 10; // Jeśli AI ma znaki w tej linii, a gracz nie ma żadnych znaków w tej linii
  }
  if (countX > 0 && countO == 0) {
    return -(countX * countX * 10); // Jeśli gracz ma znaki w tej linii, a AI nie ma żadnych znaków w tej linii
  }
  return 0; // Jeśli linia jest pusta, nie jest ona wartościowa dla żadnej ze stron
}

int evaluateBoard() {
  int score = 0;
  int center = n / 2;

  // Premia za zajmowanie pól blisko środka planszy
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      int distanceToCenter = abs(i - center) + abs(j - center);
      int proximityScore = (n - distanceToCenter) * 5;
      if (board[i][j] == 'O') {
        score += proximityScore;
      } else if (board[i][j] == 'X') {
        score -= proximityScore;
      }
    }
  }

  // Ocena linii w 4 kierunkach
  for (int i = 0; i < n; i++) {
    for (int j = 0; j <= n - toWin; j++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i][j + k] == 'O') {
          countO++;
        } else if (board[i][j + k] == 'X') {
          countX++;
        }
      }
      score += evaluateLine(countO, countX); 
    }
  }
  for (int j = 0; j < n; j++) {
    for (int i = 0; i <= n - toWin; i++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i + k][j] == 'O') {
          countO++;
        } else if (board[i + k][j] == 'X') {
          countX++;
        }
      }
      score += evaluateLine(countO, countX); 
    }
  }
  for (int i = 0; i <= n - toWin; i++) {
    for (int j = 0; j <= n - toWin; j++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i + k][j + k] == 'O') {
          countO++;
        } else if (board[i + k][j + k] == 'X') {
          countX++;
        }
      }
      score += evaluateLine(countO, countX); 
    }
  }
  for (int i = 0; i <= n - toWin; i++) {
    for (int j = toWin - 1; j < n; j++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i + k][j - k] == 'O') {
          countO++;
        } else if (board[i + k][j - k] == 'X') {
          countX++;
        }
      }
      score += evaluateLine(countO, countX); 
    }
  }
  return score;
}

// Algorytm Minimax z cięciami Alpha-Beta
int minimax(bool isMax, int alpha, int beta, int depth) {
  if (isDraw()) {
    return 0;
  }
  if (depth == 0) {
    return evaluateBoard();
  }

  auto moves = getAvailableMoves();
  if (moves.empty()) {
    return evaluateBoard();
  }
  
  if (isMax) {
    int bestScore = -1000000;
    for (auto [i, j] : moves) {
      board[i][j] = 'O';
      if (checkWin(i, j, 'O')) {
        board[i][j] = '.';
        return 100000 + depth;
      }
      int score = minimax(false, alpha, beta, depth - 1);
      board[i][j] = '.';
      bestScore = max(score, bestScore);
      alpha = max(alpha, bestScore);
      if (beta <= alpha) {
        return bestScore;
      }
    }
    return bestScore;
  } else {
    int bestScore = 1000000; 
    for (auto [i, j] : moves) {
      board[i][j] = 'X';
      if (checkWin(i, j, 'X')) {
        board[i][j] = '.';
        return -100000 - depth;
      }
      int score = minimax(true, alpha, beta, depth - 1);
      board[i][j] = '.';
      bestScore = min(score, bestScore);
      beta = min(beta, bestScore);
      if (beta <= alpha) {
        return bestScore;
      }
    }
    return bestScore;
  }
}

pair<int,int> findBestMove() {
  int bestScore = -1000000; 
  auto moves = getAvailableMoves();
  pair<int,int> bestMove = moves[0];
  
  // Tablica z maksymalną głębokością dla różnych rozmiarów planszy
  int maxDepth[] = {0,0,0, 9,7,5, 4,4,3, 3,3};
  int depth = (n <= 10) ? maxDepth[n] : 3;

  for (auto [i,j] : moves) {
    board[i][j] = 'O';
    if (checkWin(i, j, 'O')) {
      board[i][j] = '.';
      return {i, j};
    }
    board[i][j] = '.';
  }

  for (auto [i,j] : moves) {
    board[i][j] = 'X';
    if (checkWin(i, j, 'X')) {
      board[i][j] = '.';
      return {i, j};
    }
    board[i][j] = '.';
  }

  for (auto [i, j] : moves) {
    board[i][j] = 'O';
    int moveScore = minimax(false, -1000000, 1000000, depth - 1);
    board[i][j] = '.';
    if (moveScore > bestScore) {
      bestScore = moveScore;
      bestMove = {i, j};
    }
  }
  return bestMove;
}

int main() {
  cout << "======================================\n";
  cout << "       KOLKO I KRZYZYK (MINIMAX)      \n";
  cout << "======================================\n\n";

  cout << "Podaj rozmiar planszy (n x n): ";
  if (!(cin >> n)) { 
    n = 3; 
    cin.clear(); 
    cin.ignore(10000, '\n'); 
  }

  cout << "Podaj liczbe znakow do wygrania: ";
  if (!(cin >> toWin)) { 
    toWin = 3; 
    cin.clear(); 
    cin.ignore(10000, '\n'); 
  }

  int winLimit = min(n, 6);
  if (toWin > winLimit) {
    toWin = winLimit;
    cout << "Liczba znakow do wygrania ograniczona do " << winLimit << "." << endl;
  }

  int winsX = 0, winsO = 0, draws = 0;

  while (true) {
    initBoard(n);

    char startPlayer;
    cout << "Kto zaczyna? (X/O): "; 
    cin >> startPlayer;
    char currentPlayer = (toupper(startPlayer) == 'O') ? 'O' : 'X';

    int moveCount = 0;
    auto startTime = chrono::high_resolution_clock::now();
    
    while (true) {
      cout << "\n--- Stan planszy ---";
      printBoard(); 

      if (currentPlayer == 'X') {
        int row, col;
        cout << "\nRuch nr " << moveCount + 1 << " - tura gracza X\n";
        cout << "Podaj wiersz i kolumne (oddzielone spacja): ";
        
        // Obsługa nieprawidłowych danych wejściowych
        if (!(cin >> row >> col)) {
          cin.clear();
          cin.ignore(10000, '\n');
          cout << "Nieprawidlowe dane wejsciowe! Wpisz dwie liczby." << endl;
          continue;
        }

        row--;
        col--;

        if (!makeMove(row, col, currentPlayer)) {
          cout << "Nieprawidlowy ruch. Sprobuj ponownie." << endl;
          continue; 
        }
        moveCount++;

        if (checkWin(row, col, currentPlayer)) {
          printBoard();
          cout << "Gracz " << currentPlayer << " wygrywa!" << endl;
          winsX++;
          break;
        }
      } else {
        cout << "\nRuch nr " << moveCount + 1 << " - tura AI (O)...\n";
        auto aiStart = chrono::high_resolution_clock::now();
        pair<int, int> bestMove = findBestMove();
        auto aiEnd = chrono::high_resolution_clock::now();
        
        cout << "Czas ruchu AI: " << chrono::duration_cast<chrono::milliseconds>(aiEnd - aiStart).count() << " ms" << endl;
        makeMove(bestMove.first, bestMove.second, currentPlayer);
        moveCount++;

        if (checkWin(bestMove.first, bestMove.second, currentPlayer)) {
          printBoard();
          cout << "AI (O) wygrywa!" << endl;
          winsO++;
          break;
        }
      }

      if (isDraw()) {
        printBoard();
        cout << "Gra zakonczona remisem!" << endl;
        draws++;
        break;
      }

      currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    auto totalMs = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
    
    cout << "Czas gry: " << (totalMs / 1000) << "s " << (totalMs % 1000) << "ms" << endl;
    cout << "Wyniki sesji  ->  X: " << winsX << "  |  O: " << winsO << "  |  Remisy: " << draws << endl;

    char again;
    cout << "\nCzy chcesz zagrac ponownie? (t/n): ";
    cin >> again;
    if (tolower(again) != 't') {
      break; 
    }
  }

  return 0;
}