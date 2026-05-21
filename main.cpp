#include <iostream>
#include <vector>
#include <chrono>
#include <cctype>

using namespace std;

int n; // Rozmiar planszy
int toWin; // Liczba znaków do wygrania
vector<vector<char>> board; // Plansza do gry

void initBoard(int n) {
  board.assign(n, vector<char>(n, '.')); // Inicjalizacja planszy z pustymi polami
}

void printBoard() {
  cout << "\n  ";
  for (int j = 0; j < n; j++) { // Nagłówki kolumn
    cout << j + 1 << " ";
  }
  cout << "\n";

  for (int i = 0; i < n; i++) { // Nagłówki wierszy
    cout << i + 1 << " ";
    for (int j = 0; j < n; j++) {
      cout << board[i][j] << " "; // Wyświetlanie zawartości planszy
    }
    cout << "\n";
  }
}

bool makeMove(int row, int col, char player) {

  if (row < 0 || row >= n || col < 0 || col >= n) {
    return false; // Nieprawidłowe współrzędne
  }

  if (board[row][col] != '.') {
    return false; // Pole jest już zajęte
  }

  board[row][col] = player;
  return true;
}

bool checkWin(int row, int col, char player) {

  int directions[4][2] = 
  {
    {0, 1}, // Poziom
    {1, 0}, // Pion
    {1, 1}, // Przekątna "\"
    {1, -1} // Przekątna "/"
  };

  // Sprawdzenie w każdej z 4 kierunków
  for (int d = 0; d < 4; d++) {
    int dr = directions[d][0]; // Kierunek wiersza
    int dc = directions[d][1]; // Kierunek kolumny

    int count = 1; // Licznik znaków gracza

    // Sprawdzenie w pozytywnym kierunku
    int i = row + dr;
    int j = col + dc;

    while (i >= 0 && i < n && j >= 0 && j < n && board[i][j] == player) {
      count++;
      i += dr;
      j += dc;
    }

    // Sprawdzenie w negatywnym kierunku
    i = row - dr;
    j = col - dc;

    while (i >= 0 && i < n && j >= 0 && j < n && board[i][j] == player) {
      count++;
      i -= dr;
      j -= dc;
    }

    // Sprawdzenie, czy gracz wygrał
    if (count >= toWin) {
      return true; // Gracz wygrał
    }
  }
  return false; // Gracz nie wygrał
}

bool isDraw() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (board[i][j] == '.') {
        return false; // Znaleziono puste pole, więc gra nie jest remisem
      }
    }
  }
  return true; // Remis, ponieważ brak pustych pól i brak zwycięzcy
}

vector<pair<int, int>> getAvailableMoves() {

  vector<pair<int, int>> moves;

  // Dla małych plansz - sprawdzaj wszystko
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

  // Dla dużych plansz - tylko okolica
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

          if (nr >= 0 && nr < n &&
              nc >= 0 && nc < n &&
              board[nr][nc] == '.' &&
              !used[nr][nc]) {

            moves.push_back({nr, nc});
            used[nr][nc] = true;
          }
        }
      }
    }
  }

  if (moves.empty()) {
    moves.push_back({n / 2, n / 2});
  }

  return moves;
}

int evaluateLine(int countO, int countX) {

  if (countO == toWin - 1 && countX == 0) // Jeśli AI ma prawie wygraną linię, a gracz nie ma żadnych znaków w tej linii
    return 5000;

  if (countX == toWin - 1 && countO == 0) // Jeśli gracz ma prawie wygraną linię, a AI nie ma żadnych znaków w tej linii 
    return -5000;

  if (countO > 0 && countX > 0) // Jeśli obie strony mają znaki w tej linii, nie jest ona wartościowa dla żadnej ze stron
    return 0;

  if (countO > 0 && countX == 0) // Jeśli AI ma znaki w tej linii, a gracz nie ma żadnych znaków w tej linii
    return countO * countO * 10;

  if (countX > 0 && countO == 0) // Jeśli gracz ma znaki w tej linii, a AI nie ma żadnych znaków w tej linii
    return -(countX * countX * 10);

  return 0; // Jeśli linia jest pusta, nie jest ona wartościowa dla żadnej ze stron
}

int evaluateBoard() {
  int score = 0;

  int center = n / 2; // Środek planszy

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {

      int distanceToCenter = abs(i - center) + abs(j - center); // Odległość od środka
      int proximityScore = (n - distanceToCenter) * 5; // Premia za bliskość do środka

      if (board[i][j] == 'O') {
        score += proximityScore; // Premia za bliskość do środka
      } else if (board[i][j] == 'X') {
        score -= proximityScore; // Kara za bliskość do środka
      }
    }
  }

  // Ocena poziomych linii
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
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }
  // Ocena pionowych linii
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
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }
  // Ocena przekątnych "\"
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
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }

    // Ocena przekątnych "/"
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
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }
  return score;
}

int minimax(bool isMax, int alpha, int beta, int depth) {
  if (isDraw()) {
    return 0; // Remis
  }

  if (depth == 0) {
    return evaluateBoard(); // Ocena planszy na podstawie aktualnej sytuacji
  }

  if (isMax) {
    int bestScore = -100000;

    auto moves = getAvailableMoves(); // Pobranie dostępnych ruchów

    for (auto [i, j] : moves) {
      board[i][j] = 'O'; // Ruch AI

      if (checkWin(i, j, 'O')) { // Sprawdzenie wygranej AI od razu po ruchu
        board[i][j] = '.'; // Cofnięcie ruchu
        return 100000 + depth;
      }

      int score = minimax(false, alpha, beta, depth - 1); // Rekurencyjne wywołanie dla ruchu gracza
      board[i][j] = '.'; // Cofnięcie ruchu

      bestScore = max(score, bestScore); // Aktualizacja najlepszego wyniku
      alpha = max(alpha, bestScore); // Aktualizacja wartości alpha
      if (beta <= alpha) {
        return bestScore; // Przycięcie
        }
      }
    return bestScore; // Zwraca najlepszy wynik dla gracza X
  } else {
    int bestScore = 100000;
    auto moves = getAvailableMoves(); // Pobranie dostępnych ruchów

    for (auto [i, j] : moves) {
      board[i][j] = 'X'; // Ruch gracza

      if (checkWin(i, j, 'X')) { // Sprawdzenie wygranej gracza od razu po ruchu
        board[i][j] = '.'; // Cofnięcie ruchu
        return -100000 - depth;
      }

      int score = minimax(true, alpha, beta, depth - 1); // Rekurencyjne wywołanie dla ruchu AI
      board[i][j] = '.'; // Cofnięcie ruchu

      bestScore = min(score, bestScore); // Aktualizacja najlepszego wyniku
      beta = min(beta, bestScore); // Aktualizacja wartości beta
      if (beta <= alpha) {
          return bestScore; // Przycięcie
        }
      }
    return bestScore; // Zwraca najlepszy wynik dla gracza O
  }
}

pair<int, int> findBestMove() {
  int bestScore = -100000;
  pair<int, int> bestMove = {-1, -1};
  auto moves = getAvailableMoves();

  for (auto [i, j] : moves) {
    board[i][j] = 'O'; // Ruch AI
    int maxDepth[] = {0,0,0, 9,7,5, 4,4,3, 3,3};
    int depth = (n <= 10) ? maxDepth[n] : 3;
    int moveScore = minimax(false, -100000, 100000, depth - 1); // Ocena ruchu
    board[i][j] = '.'; // Cofnięcie ruchu

    if (moveScore > bestScore) {
      bestMove = {i, j}; // Aktualizacja najlepszego ruchu
      bestScore = moveScore; // Aktualizacja najlepszego wyniku
      }
    }
  return bestMove;
}


int main() {
  cout << "Podaj rozmiar planszy (n x n): ";
  cin >> n;

  cout << "Podaj liczbe znakow do wygrania: ";
  cin >> toWin;

  if (toWin > n) {
    cout << "Liczba znakow do wygrania nie moze byc wieksza niz rozmiar planszy." << endl;
    return 1; // Zakończenie programu z kodem błędu
  }

  int winsX = 0, winsO = 0, draws = 0; // Liczniki wygranych i remisów

  while (true) { // Zewnętrzna pętla ponownej gry
    initBoard(n); // Inicjalizacja planszy

    char startPlayer;
    cout << "Kto zaczyna? (X/O): ";
    cin >> startPlayer;
    char currentPlayer = (toupper(startPlayer) == 'O') ? 'O' : 'X'; // Gracz zaczyna jako wybrany znak

    int moveCount = 0; // Licznik ruchów

    auto startTime = chrono::high_resolution_clock::now(); // Start pomiaru czasu
    while (true) {
      cout << "\nRuch nr " << moveCount + 1 << ":\n"; // Wyświetlanie numeru ruchu
      printBoard(); // Wyświetlanie planszy

      if (currentPlayer == 'X') {
        int row, col;
        cout << "Ruch gracza X (podaj wiersz i kolumne): ";
        cin >> row >> col;

        row--; // Dostosowanie indeksów do 0
        col--;

        if (!makeMove(row, col, currentPlayer)) {
          cout << "Nieprawidlowy ruch. Sprobuj ponownie." << endl;
          continue; // Powtórzenie ruchu gracza
        }
        moveCount++; // Aktualizacja licznika ruchów

        if (checkWin(row, col, currentPlayer)) {
          printBoard();
          cout << "Gracz " << currentPlayer << " wygrywa!" << endl;
          winsX++; // Aktualizacja licznika wygranych gracza X
          break; // Zakończenie gry
        }
      } else {
        cout << "Ruch AI (O)..." << endl;
        auto aiStart = chrono::high_resolution_clock::now(); // Start pomiaru czasu ruchu AI
        pair<int, int> bestMove = findBestMove(); // Znalezienie najlepszego ruchu dla AI
        auto aiEnd = chrono::high_resolution_clock::now(); // Koniec pomiaru czasu ruchu AI
        cout << "Czas ruchu AI: " << chrono::duration_cast<chrono::milliseconds>(aiEnd - aiStart).count() << " ms" << endl;
        makeMove(bestMove.first, bestMove.second, currentPlayer); // Wykonanie ruchu AI
        moveCount++; // Aktualizacja licznika ruchów

        if (checkWin(bestMove.first, bestMove.second, currentPlayer)) {
          printBoard();
          cout << "Gracz " << currentPlayer << " wygrywa!" << endl;
          winsO++; // Aktualizacja licznika wygranych AI
          break; // Zakończenie gry
        }
      }

      if (isDraw()) {
        printBoard();
        cout << "Gra zakonczona remisem!" << endl;
        draws++; // Aktualizacja licznika remisów
        break; // Zakończenie gry remisem
      }

      currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; // Zmiana gracza
    }
    auto endTime = chrono::high_resolution_clock::now(); // Koniec pomiaru czasu
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime); // Obliczenie czasu trwania
    cout << "Czas gry: " << duration.count() << " ms" << endl;
    cout << "Wyniki sesji  ->  X: " << winsX << "  |  O: " << winsO << "  |  Remisy: " << draws << endl;

    char again;
    cout << "Czy chcesz zagrac ponownie? (t/n): ";
    cin >> again;
    if (tolower(again) != 't') {
      break; // Wyjście z zewnętrznej pętli
    }
  }

  return 0; // Zakończenie programu
}