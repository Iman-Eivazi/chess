# C++ Console Chess

A text-based chess game implemented in modern C++ by **Iman Eivazi**.  
The game runs entirely in the console and supports both Human vs Human and Human vs Computer play, using algebraic notation for moves (e.g. `e2 e4`).

---

## Features

- 8×8 chess board rendered in ASCII.
- Clear piece notation:
  - White pieces: `wP`, `wN`, `wB`, `wR`, `wQ`, `wK`
  - Black pieces: `bP`, `bN`, `bB`, `bR`, `bQ`, `bK`
- Two game modes:
  - **Human vs Human** (no AI)
  - **Human vs Computer** (computer plays as Black)
- Core chess rules:
  - Legal move logic for all pieces (pawn, knight, bishop, rook, queen, king)
  - No moving through pieces for sliding pieces (bishop, rook, queen)
  - Check detection
  - Checkmate detection
  - Stalemate detection (no legal moves but not in check)
  - Automatic pawn promotion to queen when reaching the last rank
- Simple AI:
  - Minimax-based search with a fixed depth (default: 2 plies)
  - Material-based evaluation function (values for P, N, B, R, Q, K)

This setup matches typical feature sets of educational console chess projects in C++ used to demonstrate OOP and basic AI search. [web:11][web:150]

---

## Project Structure

For now the project is kept as a single-source-file console program for simplicity:

- `main.cpp`
  - Contains:
    - `Color`, `Position`, `Move` types
    - `Piece` base class and derived piece types:
      - `Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King`
    - `Game` class:
      - Board representation: `std::vector<std::vector<Piece*>>`
      - Turn handling and game loop
      - Input parsing (`e2 e4` style)
      - Move validation and application
      - Check, checkmate, and stalemate logic
      - AI move selection (minimax)
    - Main menu logic:
      - Human vs Human
      - Human vs Computer (Computer = Black)

This structure resembles common single-file console game examples before they are split into headers and multiple translation units. [web:147][web:150]

---

## Build Instructions

### Prerequisites

- A C++17-compatible compiler (e.g., `g++`, `clang++`, or MSVC).
- Git (optional, for version control and GitHub).

### Build with g++ (Linux / macOS / MinGW)

In the project directory:


### Build with Visual Studio (Windows)

1. Create a new **Console App** (C++) project.
2. Replace the generated `main.cpp` content with this project’s `main.cpp`.
3. Make sure the project uses at least the `/std:c++17` standard if needed.
4. Build and run (Ctrl+F5).

These steps follow the typical workflow for small C++ console games on GitHub. [web:141][web:149]

---

## How to Play

1. Run the compiled program (`./chess` or `chess.exe`).
2. At the main menu:

   - Enter `1` for **Human vs Human**.
   - Enter `2` for **Human vs Computer** (computer plays as Black).
   - Enter `3` to exit.

3. During the game:

   - Each move is entered as:
     ```
     <from> <to>
     ```
     for example:
     ```
     e2 e4
     g1 f3
     ```
   - The first coordinate is the piece’s current square.
   - The second coordinate is the destination square.
   - To quit a running game, type:
     ```
     quit
     ```

### Coordinate System

- Files (columns): `a b c d e f g h`
- Ranks (rows): `1 2 3 4 5 6 7 8`

From White’s perspective:

- `a1` is bottom-left.
- `h1` is bottom-right.
- `a8` is top-left.
- `h8` is top-right.

The board printed in the console labels files and ranks accordingly to match this system. [web:11]

---

## Game Logic Overview

### Piece System

- `Piece`:
  - Abstract base class with:
    - `Color color() const`
    - `char typeChar() const`
    - `std::string repr() const` → two-character representation (e.g. `wQ`)
    - `bool isValidMove(...) const` → piece-specific move rules
- Derived classes:
  - `Pawn`:
    - Forward moves, double-step from starting rank, diagonal capture, promotion.
  - `Rook`:
    - Horizontal and vertical sliding moves.
  - `Bishop`:
    - Diagonal sliding moves.
  - `Knight`:
    - L-shaped jumps.
  - `Queen`:
    - Combination of rook and bishop moves.
  - `King`:
    - One square in any direction.

### Rules and Validation

- `Game::makeMove`:
  - Validates basic rules (turn, piece belongs to player, target square, piece move pattern).
  - Simulates the move and calls `isInCheck(currentPlayer_)` to ensure the king is not left in check.
  - Applies the move and captures if legal.
  - Triggers pawn promotion.

- `Game::isInCheck(Color)`:
  - Finds the king for the specified color.
  - Tests whether any opponent piece has a valid attack on the king’s square.

- `Game::generateLegalMoves(Color)`:
  - Enumerates all moves that:
    - Respect the piece’s movement rules.
    - Do not leave the moving side’s own king in check.

This design mirrors typical teaching examples of chess logic in C++ console programs. [web:11][web:44]

### AI Overview

- Evaluation function:
  - Uses basic material values:
    - Pawn = 100  
    - Knight = 320  
    - Bishop = 330  
    - Rook = 500  
    - Queen = 900  
    - King = 20000  
- Minimax:
  - Depth-limited search (default: depth 2).
  - Alternates between maximizing (AI) and minimizing (opponent).
  - Uses legal moves generated by `generateLegalMoves`.

This follows the classic “simple minimax chess AI” approach often used in beginner engines. [web:59][web:90]

---

## Roadmap / Future Work

Potential enhancements:

- Add castling (king-side and queen-side).
- Add en passant capture.
- Configurable AI difficulty (change minimax depth via menu).
- Move history and undo feature.
- Time controls or move counters.
- Split into multiple files:
  - `Piece.h/.cpp`, `Game.h/.cpp`, `main.cpp`
- Add automated tests and a CI pipeline (e.g., GitHub Actions).

These are common next steps to evolve a console chess project into a more complete and maintainable C++ application. [web:44][web:150]

---

## Author

**Iman Eivazi**

- Role: Author and maintainer of this project.
- Purpose: Learning, portfolio, and demonstration of C++ OOP and basic game AI.

Feel free to fork the project, suggest improvements, or use it as a reference for your own console-based C++ games.