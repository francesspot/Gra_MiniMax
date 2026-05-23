#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <cctype>
#include <string>

using namespace std;

int n; // Rozmiar planszy
int toWin; // Liczba znaków do wygrania
vector<vector<char>> board; // Plansza do gry

void initBoard(int n) {
  board.assign(n, vector<char>(n, '.')); // Inicjalizacja planszy z pustymi polami
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
  // Sprawdzenie w każdym z 4 kierunków
  for (int d = 0; d < 4; d++) {
    int dr = directions[d][0]; // Kierunek wiersza
    int dc = directions[d][1]; // Kierunek kolumny

    int count = 1; // Licznik znaków gracza

    // Sprawdzenie w pozytywnym kierunku
    int i = row + dr;
    int j = col + dc;
    while (i >= 0 && i < n && j >= 0 && j < n && board[i][j] == player) {
      count++; i += dr; j += dc;
    }
    // Sprawdzenie w negatywnym kierunku
    i = row - dr;
    j = col - dc;
    while (i >= 0 && i < n && j >= 0 && j < n && board[i][j] == player) {
      count++; i -= dr; j -= dc;
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

  // Dla małych plansz - sprawdzanie wszystkich pól
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

  // Dla dużych plansz - sprawdzenie tylko okolicy istniejących znaków
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
  {
    return 5000;
  }
  if (countX == toWin - 1 && countO == 0) // Jeśli gracz ma prawie wygraną linię, a AI nie ma żadnych znaków w tej linii
  {
    return -5000;
  }
  if (countO > 0 && countX > 0) // Jeśli obie strony mają znaki w tej linii, nie jest ona wartościowa dla żadnej ze stron
  {
    return 0;
  }
  if (countO > 0 && countX == 0) // Jeśli AI ma znaki w tej linii, a gracz nie ma żadnych znaków w tej linii
  {
    return countO * countO * 10;
  }
  if (countX > 0 && countO == 0) // Jeśli gracz ma znaki w tej linii, a AI nie ma żadnych znaków w tej linii
  {
    return -(countX * countX * 10);
  }
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
        if (board[i][j + k] == 'O') { countO++; }
        else if (board[i][j + k] == 'X') { countX++; }
      }
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }
  // Ocena pionowych linii
  for (int j = 0; j < n; j++) {
    for (int i = 0; i <= n - toWin; i++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i + k][j] == 'O') { countO++; }
        else if (board[i + k][j] == 'X') { countX++; }
      }
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }
  // Ocena przekątnych "\"
  for (int i = 0; i <= n - toWin; i++) {
    for (int j = 0; j <= n - toWin; j++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i + k][j + k] == 'O') { countO++; }
        else if (board[i + k][j + k] == 'X') { countX++; }
      }
      score += evaluateLine(countO, countX); // Dodanie oceny linii do całkowitego wyniku
    }
  }
  // Ocena przekątnych "/"
  for (int i = 0; i <= n - toWin; i++) {
    for (int j = toWin - 1; j < n; j++) {
      int countO = 0, countX = 0;
      for (int k = 0; k < toWin; k++) {
        if (board[i + k][j - k] == 'O') { countO++; }
        else if (board[i + k][j - k] == 'X') { countX++; }
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
  auto moves = getAvailableMoves();
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
  pair<int,int> bestMove = moves[0]; // Domyślnie pierwszy ruch jako najlepszy
  
  int maxDepth[] = {0,0,0, 9,7,5, 4,4,3, 3,3};
  int depth = (n <= 10) ? maxDepth[n] : 3;

  // Sprawdzenie, czy AI może wygrać w następnym ruchu
  for (auto [i,j] : moves) {
    board[i][j] = 'O';
    if (checkWin(i,j,'O')) { board[i][j]='.'; return {i,j}; }
    board[i][j] = '.';
  }

  // Sprawdzenie, czy gracz może wygrać w następnym ruchu i zablokowanie go
  for (auto [i,j] : moves) {
    board[i][j] = 'X';
    if (checkWin(i,j,'X')) { board[i][j]='.'; return {i,j}; }
    board[i][j] = '.';
  }

  // Wykonanie minimax dla każdego możliwego ruchu i wybór najlepszego
  for (auto [i, j] : moves) {
    board[i][j] = 'O'; 
    int moveScore = minimax(false, -1000000, 1000000, depth - 1); 
    board[i][j] = '.'; 
    if (moveScore > bestScore) {
      bestScore = moveScore; 
      bestMove = {i,j}; 
    }
  }
  return bestMove;
}

const int WINDOW_W  = 700;
const int WINDOW_H  = 780;
const int BOARD_OFF = 120;
const int MARGIN    = 30;

float cellSize() {
  return (float)(min(WINDOW_W, WINDOW_H - BOARD_OFF) - 2 * MARGIN) / n;
}

// Zamienia kliknięcie myszy na (wiersz, kolumnę) lub {-1,-1} jeśli poza planszą
pair<int,int> pixelToCell(int mx, int my) {
  float cs = cellSize();
  int col = (int)((mx - MARGIN) / cs);
  int row = (int)((my - BOARD_OFF - MARGIN) / cs);
  if (row < 0 || row >= n || col < 0 || col >= n) return {-1,-1};
  return {row, col};
}

bool clickedNewGame(int mx, int my) {
  return mx >= WINDOW_W/2-80 && mx <= WINDOW_W/2+80
      && my >= WINDOW_H-55  && my <= WINDOW_H-15;
}

void drawBoard(sf::RenderWindow& win, const sf::Font& font,
  const string& status, int winsX, int winsO, int draws, bool gameOver, int moveCount, long long aiMs, long long gameMs) {
  float cs  = cellSize();
  float bx  = MARGIN;
  float by  = BOARD_OFF + MARGIN;
  float bSz = cs * n;

  // Tło planszy
  sf::RectangleShape bg({bSz, bSz});
  bg.setPosition({bx, by});
  bg.setFillColor(sf::Color(245,245,240));
  bg.setOutlineColor(sf::Color(180,180,170));
  bg.setOutlineThickness(2.f);
  win.draw(bg);

  // Siatka
  for (int i = 1; i < n; i++) {
    sf::RectangleShape h({bSz, 1.5f});
    h.setPosition({bx, by + i*cs});
    h.setFillColor(sf::Color(180,180,170));
    win.draw(h);
    sf::RectangleShape v({1.5f, bSz});
    v.setPosition({bx + i*cs, by});
    v.setFillColor(sf::Color(180,180,170));
    win.draw(v);
  }

  // Znaki X i O
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
      if (board[r][c] == '.') continue;
      float cx  = bx + c*cs + cs/2.f;
      float cy  = by + r*cs + cs/2.f;
      float rad = cs * 0.35f;

      if (board[r][c] == 'X') {
        float off = rad * 0.75f;
        for (int t = -2; t <= 2; t++) {
          sf::Vertex l1[] = {
            sf::Vertex{{cx-off, cy-off+t}, sf::Color(210,70,50)},
            sf::Vertex{{cx+off, cy+off+t}, sf::Color(210,70,50)}
          };
          sf::Vertex l2[] = {
            sf::Vertex{{cx+off, cy-off+t}, sf::Color(210,70,50)},
            sf::Vertex{{cx-off, cy+off+t}, sf::Color(210,70,50)}
          };
          win.draw(l1, 2, sf::PrimitiveType::Lines);
          win.draw(l2, 2, sf::PrimitiveType::Lines);
        }
      } else {
        sf::CircleShape circle(rad);
        circle.setOrigin({rad, rad});
        circle.setPosition({cx, cy});
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(sf::Color(40,130,100));
        circle.setOutlineThickness(max(2.f, cs*0.06f));
        win.draw(circle);
      }
    }
  }

  // Status (tura / wynik)
  unsigned int fs = (unsigned int)max(16, min(26, 500/n));
  sf::Text st(font, status, fs);
  st.setFillColor(sf::Color(50,50,50));
  auto sr = st.getLocalBounds();
  st.setOrigin({sr.size.x/2.f, sr.size.y/2.f});
  st.setPosition({WINDOW_W/2.f, 28.f});
  win.draw(st);

  // Wyniki sesji + numer ruchu
  string sc = "X: "+to_string(winsX)+"  Remisy: "+to_string(draws)+"  O: "+to_string(winsO) + "     Ruch nr: "+to_string(moveCount+1);
  sf::Text scT(font, sc, 16);
  scT.setFillColor(sf::Color(100,100,100));
  auto scr = scT.getLocalBounds();
  scT.setOrigin({scr.size.x/2.f, scr.size.y/2.f});
  scT.setPosition({WINDOW_W/2.f, 68.f});
  win.draw(scT);

  // Czas ostatniego ruchu AI
  if (aiMs >= 0) {
    string aiStr = "Czas ruchu AI: " + to_string(aiMs) + " ms";
    sf::Text aiT(font, aiStr, 14);
    aiT.setFillColor(sf::Color(120,120,120));
    auto ar = aiT.getLocalBounds();
    aiT.setOrigin({ar.size.x/2.f, ar.size.y/2.f});
    aiT.setPosition({WINDOW_W/2.f, 93.f});
    win.draw(aiT);
  }

  // Przycisk nowej gry + czas gry na ekranie końcowym
  if (gameOver) {
    // Czas gry
    string timeStr = "Czas gry: " + to_string(gameMs / 1000) + " s " + to_string(gameMs % 1000) + " ms";
    sf::Text timeT(font, timeStr, 16);
    timeT.setFillColor(sf::Color(120,120,120));
    auto tr = timeT.getLocalBounds();
    timeT.setOrigin({tr.size.x/2.f, tr.size.y/2.f});
    timeT.setPosition({WINDOW_W/2.f, (float)(WINDOW_H-75)});
    win.draw(timeT);

    sf::RectangleShape btn({160.f, 40.f});
    btn.setOrigin({80.f, 20.f});
    btn.setPosition({WINDOW_W/2.f, (float)(WINDOW_H-35)});
    btn.setFillColor(sf::Color(70,70,70));
    btn.setOutlineColor(sf::Color(150,150,150));
    btn.setOutlineThickness(1.f);
    win.draw(btn);
    sf::Text btnT(font, "Nowa gra", 18);
    auto br = btnT.getLocalBounds();
    btnT.setOrigin({br.size.x/2.f, br.size.y/2.f});
    btnT.setPosition({WINDOW_W/2.f, (float)(WINDOW_H-36)});
    btnT.setFillColor(sf::Color::White);
    win.draw(btnT);
  }
}

int main() {
  sf::RenderWindow window(
    sf::VideoMode({(unsigned)WINDOW_W, (unsigned)WINDOW_H}),
    "Kolko i Krzyzyk"
  );
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
    cerr << "Brak fontu arial.ttf\n";
    return 1;
  }

  // Ekran konfiguracji — wybór rozmiaru planszy i liczby znaków
  n     = 3;
  toWin = 3;
  bool configuring = true;

  // Pomocnicza funkcja rysująca przycisk +/-
  auto drawPlusMinus = [&](int val, float cx, float cy, const string& label) {
    // Etykieta
    sf::Text lbl(font, label, 18);
    auto lb = lbl.getLocalBounds();
    lbl.setOrigin({lb.size.x/2.f, lb.size.y/2.f});
    lbl.setPosition({cx, cy - 45.f});
    lbl.setFillColor(sf::Color(80,80,80));
    window.draw(lbl);

    // Przycisk -
    sf::RectangleShape btnM({40.f, 40.f});
    btnM.setOrigin({20.f, 20.f});
    btnM.setPosition({cx - 60.f, cy});
    btnM.setFillColor(sf::Color(180,80,60));
    window.draw(btnM);
    sf::Text tM(font, "-", 24);
    auto mb2 = tM.getLocalBounds();
    tM.setOrigin({mb2.size.x/2.f, mb2.size.y/2.f});
    tM.setPosition({cx - 60.f, cy - 2.f});
    tM.setFillColor(sf::Color::White);
    window.draw(tM);

    // Wartość
    sf::Text tV(font, to_string(val), 28);
    auto vb = tV.getLocalBounds();
    tV.setOrigin({vb.size.x/2.f, vb.size.y/2.f});
    tV.setPosition({cx, cy});
    tV.setFillColor(sf::Color(40,40,40));
    window.draw(tV);

    // Przycisk +
    sf::RectangleShape btnP({40.f, 40.f});
    btnP.setOrigin({20.f, 20.f});
    btnP.setPosition({cx + 60.f, cy});
    btnP.setFillColor(sf::Color(60,140,100));
    window.draw(btnP);
    sf::Text tP(font, "+", 24);
    auto pb = tP.getLocalBounds();
    tP.setOrigin({pb.size.x/2.f, pb.size.y/2.f});
    tP.setPosition({cx + 60.f, cy - 2.f});
    tP.setFillColor(sf::Color::White);
    window.draw(tP);
  };

  int winsX = 0, winsO = 0, draws = 0; // Liczniki wygranych i remisów

  // Zmienne stanu gry
  char   currentPlayer;
  string status;
  bool   gameOver;
  bool   aiThinking;
  int    moveCount;
  long long aiMs   = -1;  // Czas ostatniego ruchu AI w milisekundach (-1 oznacza jeszcze nie grało)
  long long gameMs =  0;  // Czas trwania całej gry
  bool   choosingPlayer = false;

  chrono::high_resolution_clock::time_point startTime;

  auto resetGame = [&](char starter) {
    initBoard(n);
    currentPlayer = starter; // Gracz zaczyna jako wybrany znak
    status        = (starter == 'X') ? "Tura gracza X" : "AI (O) mysli...";
    gameOver      = false;
    aiThinking    = (starter == 'O');
    moveCount     = 0;
    aiMs          = -1;
    startTime     = chrono::high_resolution_clock::now(); // Start pomiaru czasu gry
  };
  resetGame('X');

  while (window.isOpen()) {
    while (const auto ev = window.pollEvent()) {
      if (ev->is<sf::Event::Closed>()) window.close();

      if (const auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
          int mx = mb->position.x, my = mb->position.y;

          // Ekran konfiguracji — przyciski +/-
          if (configuring) {
            float cx1 = WINDOW_W/2.f - 120.f; // centrum kontrolki rozmiaru
            float cx2 = WINDOW_W/2.f + 120.f; // centrum kontrolki znaków
            float cy  = 340.f;
            // Rozmiar planszy -
            if (mx>=cx1-80&&mx<=cx1-40&&my>=cy-20&&my<=cy+20) { if(n>3){n--;if(toWin>n)toWin=n;} }
            // Rozmiar planszy +
            if (mx>=cx1+40&&mx<=cx1+80&&my>=cy-20&&my<=cy+20) { if(n<10)n++; }
            // Znaki do wygranej -
            if (mx>=cx2-80&&mx<=cx2-40&&my>=cy-20&&my<=cy+20) { if(toWin>3)toWin--; }
            // Znaki do wygranej +
            if (mx>=cx2+40&&mx<=cx2+80&&my>=cy-20&&my<=cy+20) { if(toWin<min(n,6))toWin++; }
            // Przycisk Dalej
            if (mx>=WINDOW_W/2-80&&mx<=WINDOW_W/2+80&&my>=450&&my<=500) {
              configuring = false; choosingPlayer = true;
            }
            continue;
          }

          // Ekran wyboru kto zaczyna
          if (choosingPlayer) {
            // Przycisk X (lewy)
            if (mx >= WINDOW_W/2-170 && mx <= WINDOW_W/2-10 && my >= 340 && my <= 400) {
              resetGame('X'); choosingPlayer = false;
            }
            // Przycisk O (prawy)
            if (mx >= WINDOW_W/2+10 && mx <= WINDOW_W/2+170 && my >= 340 && my <= 400) {
              resetGame('O'); choosingPlayer = false;
            }
            continue;
          }

          // Nowa gra po kliknięciu przycisku
          if (gameOver && clickedNewGame(mx, my)) {
            configuring = true; choosingPlayer = false;
            continue;
          }

          // Ruch gracza X
          if (!gameOver && !aiThinking && currentPlayer == 'X') {
            auto [row, col] = pixelToCell(mx, my);
            if (row == -1) continue;
            if (!makeMove(row, col, 'X')) continue;
            moveCount++; // Aktualizacja licznika ruchów

            if (checkWin(row, col, 'X')) {
              gameMs = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();
              status = "Gracz X wygrywa!"; winsX++; gameOver = true; // Aktualizacja licznika wygranych gracza X
            } else if (isDraw()) {
              gameMs = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();
              status = "Remis!"; draws++; gameOver = true; // Aktualizacja licznika remisów
            } else {
              currentPlayer = 'O'; status = "AI (O) mysli..."; aiThinking = true;
            }
          }
        }
      }
    }

    // Ekran wyboru kto zaczyna
    if (choosingPlayer) {
      window.clear(sf::Color(235,235,228));

      sf::Text title(font, "Kto zaczyna?", 30);
      auto tr = title.getLocalBounds();
      title.setOrigin({tr.size.x/2.f, tr.size.y/2.f});
      title.setPosition({WINDOW_W/2.f, 260.f});
      title.setFillColor(sf::Color(50,50,50));
      window.draw(title);

      // Przycisk X
      sf::RectangleShape btnX({160.f, 60.f});
      btnX.setPosition({(float)(WINDOW_W/2-170), 340.f});
      btnX.setFillColor(sf::Color(210,70,50));
      btnX.setOutlineColor(sf::Color(180,50,30));
      btnX.setOutlineThickness(1.f);
      window.draw(btnX);
      sf::Text txtX(font, "Gracz X", 22);
      auto xr = txtX.getLocalBounds();
      txtX.setOrigin({xr.size.x/2.f, xr.size.y/2.f});
      txtX.setPosition({(float)(WINDOW_W/2-90), 370.f});
      txtX.setFillColor(sf::Color::White);
      window.draw(txtX);

      // Przycisk O (AI)
      sf::RectangleShape btnO({160.f, 60.f});
      btnO.setPosition({(float)(WINDOW_W/2+10), 340.f});
      btnO.setFillColor(sf::Color(40,130,100));
      btnO.setOutlineColor(sf::Color(20,100,70));
      btnO.setOutlineThickness(1.f);
      window.draw(btnO);
      sf::Text txtO(font, "AI (O)", 22);
      auto or2 = txtO.getLocalBounds();
      txtO.setOrigin({or2.size.x/2.f, or2.size.y/2.f});
      txtO.setPosition({(float)(WINDOW_W/2+90), 370.f});
      txtO.setFillColor(sf::Color::White);
      window.draw(txtO);

      window.display();
      continue;
    }

    // Ruch AI - wykonywany poza pętlą zdarzeń, aby uniknąć blokowania interfejsu podczas myślenia AI
    if (aiThinking && !gameOver) {
      auto aiStart = chrono::high_resolution_clock::now(); // Start pomiaru czasu ruchu AI
      auto [r, c] = findBestMove(); // Znalezienie najlepszego ruchu dla AI
      aiMs = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - aiStart).count(); // Koniec pomiaru czasu ruchu AI
      makeMove(r, c, 'O'); // Wykonanie ruchu AI
      moveCount++; // Aktualizacja licznika ruchów
      aiThinking = false;

      if (checkWin(r, c, 'O')) {
        gameMs = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();
        status = "AI (O) wygrywa!"; winsO++; gameOver = true; // Aktualizacja licznika wygranych AI
      } else if (isDraw()) {
        gameMs = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();
        status = "Remis!"; draws++; gameOver = true; // Aktualizacja licznika remisów
      } else {
        currentPlayer = 'X'; status = "Tura gracza X"; // Zmiana gracza
      }
    }

    window.clear(sf::Color(235,235,228));
    if (configuring) {
      drawPlusMinus(n, WINDOW_W/2.f - 120.f, 340.f, "Rozmiar planszy");
      drawPlusMinus(toWin, WINDOW_W/2.f + 120.f, 340.f, "Znakow do wygranej");
      
      sf::RectangleShape btnN({160.f, 50.f});
      btnN.setOrigin({80.f, 25.f});
      btnN.setPosition({WINDOW_W/2.f, 475.f});
      btnN.setFillColor(sf::Color(60,120,180));
      window.draw(btnN);
      sf::Text tN(font, "Dalej", 22);
      auto nr = tN.getLocalBounds();
      tN.setOrigin({nr.size.x/2.f, nr.size.y/2.f});
      tN.setPosition({WINDOW_W/2.f, 472.f});
      tN.setFillColor(sf::Color::White);
      window.draw(tN);
    } else {
      drawBoard(window, font, status, winsX, winsO, draws, gameOver, moveCount, aiMs, gameMs);
    }
    window.display();
  }
  return 0; // Zakończenie programu
}