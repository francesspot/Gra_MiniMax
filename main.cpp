#include <iostream>
#include <vector>

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

char checkWinner() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (board[i][j] != '.' && checkWin(i, j, board[i][j])) {
        return board[i][j]; // Zwraca znak gracza, który wygrał
      }
    }
  }
  return '.'; // Brak wygranego gracza
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

int minimax(bool isMax) {
  char winner = checkWinner();

  if (winner == 'X') {
    return -10; // Gracz X wygrał
  } else if (winner == 'O') {
    return 10; // Gracz O wygrał
  } else if (isDraw()) {
    return 0; // Remis
  }

  if (isMax) {
    int bestScore = -1000;

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (board[i][j] == '.') {
          board[i][j] = 'O'; // Ruch AI
          int score = minimax(false); // Rekurencyjne wywołanie dla ruchu gracza
          board[i][j] = '.'; // Cofnięcie ruchu

          bestScore = max(score, bestScore); // Aktualizacja najlepszego wyniku
        }
      }
    }
    return bestScore; // Zwraca najlepszy wynik dla gracza X
  }
  else {
    int bestScore = 1000;

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (board[i][j] == '.') {
          board[i][j] = 'X'; // Ruch gracza
          int score = minimax(true); // Rekurencyjne wywołanie dla ruchu AI
          board[i][j] = '.'; // Cofnięcie ruchu

          bestScore = min(score, bestScore); // Aktualizacja najlepszego wyniku
        }
      }
    }
    return bestScore; // Zwraca najlepszy wynik dla gracza O
  }
}

pair<int, int> findBestMove() {
  int bestScore = -1000;
  pair<int, int> bestMove = {-1, -1};

  for (int i = 0; i < n; i++ ) {
    for (int j = 0; j < n; j++) {
      if (board[i][j] == '.') {
        board[i][j] = 'O'; // Ruch AI
        int moveScore = minimax(false); // Ocena ruchu
        board[i][j] = '.'; // Cofnięcie ruchu

        if (moveScore > bestScore) {
          bestMove = {i, j}; // Aktualizacja najlepszego ruchu
          bestScore = moveScore; // Aktualizacja najlepszego wyniku
        }
      }
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

  initBoard(n); // Inicjalizacja planszy

  char currentPlayer = 'X'; // Gracz zaczyna jako 'X'

  while (true) {
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

      if (checkWin(row, col, currentPlayer)) {
        printBoard();
        cout << "Gracz " << currentPlayer << " wygrywa!" << endl;
        break; // Zakończenie gry
      }
    } else {
      cout << "Ruch AI (O)..." << endl;
      pair<int, int> bestMove = findBestMove(); // Znalezienie najlepszego ruchu dla AI
      makeMove(bestMove.first, bestMove.second, currentPlayer); // Wykonanie ruchu AI

      if (checkWin(bestMove.first, bestMove.second, currentPlayer)) {
        printBoard();
        cout << "Gracz " << currentPlayer << " wygrywa!" << endl;
        break; // Zakończenie gry
      }
    }

    if (isDraw()) {
      printBoard();
      cout << "Gra zakonczona remisem!" << endl;
      break; // Zakończenie gry remisem
    }

    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; // Zmiana gracza
  }
  return 0; // Zakończenie programu
}