**Polski** | [English](README.en.md)

# Kółko i krzyżyk z AI (MiniMax)

Uogólniona gra w kółko i krzyżyk w C++, w której grasz jako **X** przeciwko komputerowi (**O**) sterowanemu **algorytmem MiniMax z obcinaniem alfa-beta**. Rozmiar planszy i liczbę znaków w rzędzie potrzebnych do wygranej można ustawić.

W repozytorium są dwie wersje:

- **`main.cpp`** — wersja konsolowa,
- **`main_sfml.cpp`** — wersja graficzna w **SFML 3** (sterowanie myszą, przyciski +/− do ustawień, licznik wyników, czas ruchu AI i czas gry).

## Zasady i ustawienia

- Plansza **n × n**, od 3 × 3 do 10 × 10 (wersja konsolowa przyjmuje dowolny rozmiar).
- Liczba znaków w rzędzie do wygranej: od 3 do `min(n, 6)`.
- Liczą się linie poziome, pionowe i obie przekątne.
- Wybierasz, kto zaczyna; gra zlicza wygrane X, wygrane O i remisy.

## Jak działa AI

1. Jeśli AI może wygrać jednym ruchem — wykonuje go.
2. Jeśli gracz mógłby wygrać w następnym ruchu — AI go blokuje.
3. W pozostałych przypadkach uruchamia MiniMax z obcinaniem alfa-beta. Głębokość przeszukiwania zależy od rozmiaru planszy (9 dla 3 × 3, aż do 3 dla plansz 8 × 8 i większych), żeby ruch trwał rozsądnie długo.
4. Po osiągnięciu limitu głębokości pozycja jest oceniana heurystyką, która nagradza linie zawierające znaki tylko jednego gracza (im więcej znaków, tym wyższa ocena) oraz pola blisko środka planszy.

Po każdym ruchu AI wyświetlany jest czas, jaki na niego potrzebowało.

## Kompilacja i uruchomienie

### Wersja konsolowa

```bash
g++ -std=c++17 -O2 main.cpp -o main
./main
```

### Wersja graficzna (SFML 3)

```bash
g++ -std=c++17 -O2 main_sfml.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system
./game
```

Plik `arial.ttf` musi być w katalogu roboczym. Na Windowsie repozytorium zawiera już biblioteki DLL SFML 3 i zbudowany `game.exe`.
