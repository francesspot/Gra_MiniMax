🇵🇱 **Polski** | 🇬🇧 [English](README.en.md)

# ❌⭕ Kółko i krzyżyk z AI (MiniMax)

![C++](https://img.shields.io/badge/C++-00599C?logo=cplusplus&logoColor=white) ![SFML](https://img.shields.io/badge/SFML_3-8CC445?logo=sfml&logoColor=white)

> 🎓 Projekt na przedmiot **Projektowanie i Analiza Algorytmów** (Politechnika Wrocławska)

Uogólnione kółko i krzyżyk z przeciwnikiem komputerowym opartym na algorytmie **MiniMax z obcinaniem alfa-beta**. Plansza od 3×3 do 10×10, do wygranej potrzeba od 3 do 6 znaków w rzędzie.

## ✨ Co zrobiłem

- **AI** — najpierw sprawdza, czy może wygrać albo musi zablokować gracza, a potem przeszukuje drzewo gry MiniMaxem z cięciami alfa-beta
- **Głębokość dopasowana do planszy** — 9 ruchów w przód na 3×3, mniej na dużych planszach, żeby AI myślało szybko
- **Heurystyka** oceny pozycji — liczy linie, które może jeszcze wygrać każdy gracz, i premiuje środek planszy
- Dwie wersje: **konsolowa** i **graficzna w SFML 3** (sterowanie myszą, licznik wyników, czas ruchu AI)

## 🛠️ Technologie

C++17 · SFML 3

## 🚀 Uruchomienie

```bash
# wersja konsolowa
g++ -std=c++17 -O2 main.cpp -o main && ./main

# wersja graficzna (wymaga SFML 3)
g++ -std=c++17 -O2 main_sfml.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system && ./game
```

Dla Windowsa w repozytorium jest gotowy `game.exe` wraz z bibliotekami SFML.
