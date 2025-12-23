/*
 * Console Chess Game in C++
 *
 * Author: Iman Eivazi
 * Description:
 *   Text-based chess game with:
 *     - Human vs Human mode
 *     - Human vs Computer mode (simple minimax AI)
 *   Board coordinates use algebraic notation (e.g. "e2 e4").
 */

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>

 // -----------------------------------------------------------------------------
 // Basic types
 // -----------------------------------------------------------------------------

 // Represents the side a piece belongs to.
enum class Color {
    White,
    Black,
    None
};

// Represents a position on the board.
// row: 0..7  (0 = rank 1, 7 = rank 8)
// col: 0..7  (0 = file a, 7 = file h)
struct Position {
    int row;
    int col;
};

// Encapsulates a move from one board position to another.
struct Move {
    Position from;
    Position to;
};

// -----------------------------------------------------------------------------
// Piece base class and derived piece types
// -----------------------------------------------------------------------------

class Piece {
public:
    explicit Piece(Color color) : color_(color) {}
    virtual ~Piece() = default;

    // Returns the color of this piece.
    Color color() const { return color_; }

    // Returns the single-character type for this piece (P, N, B, R, Q, K).
    virtual char typeChar() const = 0;

    // Returns a two-character representation: "wP", "bQ", or "..".
    std::string repr() const {
        if (color_ == Color::White) return std::string("w") + typeChar();
        if (color_ == Color::Black) return std::string("b") + typeChar();
        return "..";
    }

    // Checks whether a move is valid for this piece type,
    // ignoring "moving into check" rules.
    virtual bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& board) const = 0;

protected:
    // Utility function: checks that the path between two squares is empty.
    // Used by sliding pieces like rooks, bishops, and queens.
    bool isPathClear(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& board) const {
        int dRow = (to.row > from.row) - (to.row < from.row);
        int dCol = (to.col > from.col) - (to.col < from.col);

        int r = from.row + dRow;
        int c = from.col + dCol;
        while (r != to.row || c != to.col) {
            if (board[r][c] != nullptr) {
                return false;
            }
            r += dRow;
            c += dCol;
        }
        return true;
    }

private:
    Color color_;
};

class Pawn : public Piece {
public:
    explicit Pawn(Color color) : Piece(color) {}
    char typeChar() const override { return 'P'; }

    // Implements standard pawn moves:
    //   - one step forward (if empty)
    //   - two steps from starting rank (if empty)
    //   - diagonal capture one step forward
    bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& board) const override {
        int dir = (color() == Color::White) ? 1 : -1;
        int startRow = (color() == Color::White) ? 1 : 6;

        int dRow = to.row - from.row;
        int dCol = to.col - from.col;

        // Forward moves (no capture).
        if (dCol == 0) {
            // Single step.
            if (dRow == dir && board[to.row][to.col] == nullptr) {
                return true;
            }
            // Double step from starting row.
            if (from.row == startRow && dRow == 2 * dir) {
                int midRow = from.row + dir;
                if (board[midRow][from.col] == nullptr &&
                    board[to.row][to.col] == nullptr) {
                    return true;
                }
            }
        }

        // Diagonal capture.
        if (std::abs(dCol) == 1 && dRow == dir) {
            if (board[to.row][to.col] != nullptr &&
                board[to.row][to.col]->color() != color()) {
                return true;
            }
        }

        return false;
    }
};

class Rook : public Piece {
public:
    explicit Rook(Color color) : Piece(color) {}
    char typeChar() const override { return 'R'; }

    // Rook moves horizontally or vertically over empty squares.
    bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& board) const override {
        if (from.row != to.row && from.col != to.col) {
            return false;
        }
        return isPathClear(from, to, board);
    }
};

class Bishop : public Piece {
public:
    explicit Bishop(Color color) : Piece(color) {}
    char typeChar() const override { return 'B'; }

    // Bishop moves diagonally over empty squares.
    bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& board) const override {
        int dRow = std::abs(to.row - from.row);
        int dCol = std::abs(to.col - from.col);
        if (dRow != dCol) {
            return false;
        }
        return isPathClear(from, to, board);
    }
};

class Knight : public Piece {
public:
    explicit Knight(Color color) : Piece(color) {}
    char typeChar() const override { return 'N'; }

    // Knight moves in an 'L' shape: (2,1) or (1,2).
    bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& /*board*/) const override {
        int dRow = std::abs(to.row - from.row);
        int dCol = std::abs(to.col - from.col);
        return (dRow == 2 && dCol == 1) || (dRow == 1 && dCol == 2);
    }
};

class Queen : public Piece {
public:
    explicit Queen(Color color) : Piece(color) {}
    char typeChar() const override { return 'Q'; }

    // Queen combines rook and bishop moves.
    bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& board) const override {
        int dRow = std::abs(to.row - from.row);
        int dCol = std::abs(to.col - from.col);

        // Rook-like movement.
        if (from.row == to.row || from.col == to.col) {
            return isPathClear(from, to, board);
        }

        // Bishop-like movement.
        if (dRow == dCol) {
            return isPathClear(from, to, board);
        }

        return false;
    }
};

class King : public Piece {
public:
    explicit King(Color color) : Piece(color) {}
    char typeChar() const override { return 'K'; }

    // King moves one square in any direction.
    bool isValidMove(const Position& from,
        const Position& to,
        const std::vector<std::vector<Piece*>>& /*board*/) const override {
        int dRow = std::abs(to.row - from.row);
        int dCol = std::abs(to.col - from.col);
        return dRow <= 1 && dCol <= 1;
    }
};

// -----------------------------------------------------------------------------
// Game class
// -----------------------------------------------------------------------------

class Game {
public:
    // aiSide:
    //   - Color::None  => Human vs Human
    //   - Color::Black => Human (White) vs Computer (Black)
    explicit Game(Color aiSide)
        : currentPlayer_(Color::White),
        gameOver_(false),
        aiPlays_(aiSide) {
        board_.resize(8, std::vector<Piece*>(8, nullptr));
        setupBoard();
    }

    ~Game() {
        for (auto& row : board_) {
            for (Piece* p : row) {
                delete p;
            }
        }
    }

    // Main game loop. Handles turns until checkmate, stalemate, or user exit.
    void run() {
        while (!gameOver_) {
            printBoard();

            if (isInCheck(currentPlayer_)) {
                std::cout << ">>> " << colorName(currentPlayer_)
                    << " is in CHECK!\n";
            }

            auto moves = generateLegalMoves(currentPlayer_);
            if (moves.empty()) {
                if (isInCheck(currentPlayer_)) {
                    std::cout << ">>> CHECKMATE! "
                        << colorName(opponent(currentPlayer_))
                        << " wins.\n";
                }
                else {
                    std::cout << ">>> STALEMATE! Draw.\n";
                }
                break;
            }

            if (currentPlayer_ == aiPlays_) {
                std::cout << "[AI " << colorName(currentPlayer_) << "] thinking...\n";
                Move best = findBestMove(aiPlays_, 2);
                applyMove(best);
                std::cout << "[AI] " << toNotation(best.from)
                    << " -> " << toNotation(best.to) << "\n";
            }
            else {
                std::cout << "Turn: " << colorName(currentPlayer_) << "\n";
                std::cout << "Enter move (e.g. e2 e4) or \"quit\": ";

                std::string fromStr;
                std::string toStr;

                if (!(std::cin >> fromStr)) {
                    return;
                }
                if (fromStr == "quit") {
                    std::cout << "Game ended by user.\n";
                    break;
                }
                if (!(std::cin >> toStr)) {
                    return;
                }

                Position from;
                Position to;
                if (!parsePosition(fromStr, from) ||
                    !parsePosition(toStr, to)) {
                    std::cout << "Invalid input. Use format like e2 e4.\n";
                    continue;
                }

                if (!makeMove(from, to)) {
                    std::cout << "Illegal move. Try again.\n";
                    continue;
                }
            }

            currentPlayer_ = opponent(currentPlayer_);
        }
    }

private:
    std::vector<std::vector<Piece*>> board_;
    Color currentPlayer_;
    bool gameOver_;
    Color aiPlays_;

    // -------------------------------------------------------------------------
    // Board setup and display
    // -------------------------------------------------------------------------

    // Places all pieces in their initial positions.
    void setupBoard() {
        for (auto& row : board_) {
            for (Piece*& p : row) {
                delete p;
                p = nullptr;
            }
        }

        // Pawns.
        for (int c = 0; c < 8; ++c) {
            board_[1][c] = new Pawn(Color::White); // rank 2
            board_[6][c] = new Pawn(Color::Black); // rank 7
        }

        // Rooks.
        board_[0][0] = new Rook(Color::White);
        board_[0][7] = new Rook(Color::White);
        board_[7][0] = new Rook(Color::Black);
        board_[7][7] = new Rook(Color::Black);

        // Knights.
        board_[0][1] = new Knight(Color::White);
        board_[0][6] = new Knight(Color::White);
        board_[7][1] = new Knight(Color::Black);
        board_[7][6] = new Knight(Color::Black);

        // Bishops.
        board_[0][2] = new Bishop(Color::White);
        board_[0][5] = new Bishop(Color::White);
        board_[7][2] = new Bishop(Color::Black);
        board_[7][5] = new Bishop(Color::Black);

        // Queens.
        board_[0][3] = new Queen(Color::White);
        board_[7][3] = new Queen(Color::Black);

        // Kings.
        board_[0][4] = new King(Color::White);
        board_[7][4] = new King(Color::Black);
    }

    // Renders the board to the console in a human-readable way.
    void printBoard() const {
        std::cout << "\n========== CHESS ==========\n";
        std::cout << "     a   b   c   d   e   f   g   h\n";
        std::cout << "   +---+---+---+---+---+---+---+---+\n";

        for (int r = 7; r >= 0; --r) {
            std::cout << " " << r + 1 << " |";
            for (int c = 0; c < 8; ++c) {
                if (board_[r][c] == nullptr) {
                    std::cout << " ..|";
                }
                else {
                    std::cout << " " << board_[r][c]->repr() << "|";
                }
            }
            std::cout << " " << r + 1 << "\n";
            std::cout << "   +---+---+---+---+---+---+---+---+\n";
        }

        std::cout << "     a   b   c   d   e   f   g   h\n\n";
    }

    // -------------------------------------------------------------------------
    // Input / output helpers
    // -------------------------------------------------------------------------

    // Converts a string like "e2" into a Position.
    static bool parsePosition(const std::string& s, Position& pos) {
        if (s.size() != 2) {
            return false;
        }

        char file = std::tolower(static_cast<unsigned char>(s[0]));
        char rank = s[1];

        if (file < 'a' || file > 'h') {
            return false;
        }
        if (rank < '1' || rank > '8') {
            return false;
        }

        pos.col = file - 'a';
        pos.row = rank - '1';
        return true;
    }

    // Converts a Position back into algebraic notation (e.g. e2).
    static std::string toNotation(const Position& pos) {
        std::string s;
        s.push_back(static_cast<char>('a' + pos.col));
        s.push_back(static_cast<char>('1' + pos.row));
        return s;
    }

    // -------------------------------------------------------------------------
    // Move application and legality
    // -------------------------------------------------------------------------

    // Checks and executes a move for the current player.
    bool makeMove(const Position& from, const Position& to) {
        if (!inBounds(from) || !inBounds(to)) {
            return false;
        }

        Piece* moving = board_[from.row][from.col];
        if (!moving) {
            return false;
        }

        if (moving->color() != currentPlayer_) {
            return false;
        }

        if (from.row == to.row && from.col == to.col) {
            return false;
        }

        Piece* target = board_[to.row][to.col];
        if (target && target->color() == currentPlayer_) {
            return false;
        }

        if (!moving->isValidMove(from, to, board_)) {
            return false;
        }

        // Simulate move to ensure it does not leave own king in check.
        board_[to.row][to.col] = moving;
        board_[from.row][from.col] = nullptr;

        bool kingInCheck = isInCheck(currentPlayer_);

        // Revert simulation.
        board_[from.row][from.col] = moving;
        board_[to.row][to.col] = target;

        if (kingInCheck) {
            return false;
        }

        // Apply real move and remove captured piece if any.
        if (target) {
            delete target;
        }
        board_[to.row][to.col] = moving;
        board_[from.row][from.col] = nullptr;

        handlePromotion(to);
        return true;
    }

    // Executes a move without additional legality checks (used by AI).
    void applyMove(const Move& m) {
        Piece* moving = board_[m.from.row][m.from.col];
        Piece* target = board_[m.to.row][m.to.col];

        if (target) {
            delete target;
        }

        board_[m.to.row][m.to.col] = moving;
        board_[m.from.row][m.from.col] = nullptr;

        handlePromotion(m.to);
    }

    // Automatically promotes a pawn reaching the last rank to a queen.
    void handlePromotion(const Position& to) {
        Piece* p = board_[to.row][to.col];
        if (!p) {
            return;
        }

        char t = p->typeChar();
        bool isWhitePawn = (t == 'P' && p->color() == Color::White);
        bool isBlackPawn = (t == 'P' && p->color() == Color::Black);

        if (isWhitePawn && to.row == 7) {
            delete p;
            board_[to.row][to.col] = new Queen(Color::White);
        }
        else if (isBlackPawn && to.row == 0) {
            delete p;
            board_[to.row][to.col] = new Queen(Color::Black);
        }
    }

    // -------------------------------------------------------------------------
    // Check and move generation
    // -------------------------------------------------------------------------

    // Returns true if the given color's king is under attack.
    bool isInCheck(Color color) const {
        Position kingPos;
        if (!findKing(color, kingPos)) {
            return false;
        }

        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Piece* p = board_[r][c];
                if (p && p->color() == opponent(color)) {
                    Position from{ r, c };
                    if (p->isValidMove(from, kingPos, board_)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // Finds the king of the given color on the board.
    bool findKing(Color color, Position& pos) const {
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Piece* p = board_[r][c];
                if (p && p->color() == color && p->typeChar() == 'K') {
                    pos = Position{ r, c };
                    return true;
                }
            }
        }
        return false;
    }

    // Generates all legal moves for a given side (moves that do not leave the king in check).
    std::vector<Move> generateLegalMoves(Color side) {
        std::vector<Move> moves;

        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Piece* p = board_[r][c];
                if (!p || p->color() != side) {
                    continue;
                }

                Position from{ r, c };

                for (int rr = 0; rr < 8; ++rr) {
                    for (int cc = 0; cc < 8; ++cc) {
                        Position to{ rr, cc };
                        if (from.row == to.row && from.col == to.col) {
                            continue;
                        }

                        Piece* target = board_[rr][cc];
                        if (target && target->color() == side) {
                            continue;
                        }

                        if (!p->isValidMove(from, to, board_)) {
                            continue;
                        }

                        // Simulate move.
                        board_[rr][cc] = p;
                        board_[r][c] = nullptr;

                        bool inCheck = isInCheck(side);

                        // Undo simulation.
                        board_[r][c] = p;
                        board_[rr][cc] = target;

                        if (!inCheck) {
                            moves.push_back({ from, to });
                        }
                    }
                }
            }
        }

        return moves;
    }

    // -------------------------------------------------------------------------
    // Evaluation and AI (minimax)
    // -------------------------------------------------------------------------

    // Returns a simple material value for a piece type.
    static int pieceValue(char t) {
        switch (t) {
        case 'P': return 100;
        case 'N': return 320;
        case 'B': return 330;
        case 'R': return 500;
        case 'Q': return 900;
        case 'K': return 20000;
        default:  return 0;
        }
    }

    // Evaluates the board from the perspective of sideToMove.
    int evaluateBoard(Color sideToMove) {
        int score = 0;

        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Piece* p = board_[r][c];
                if (!p) {
                    continue;
                }

                int val = pieceValue(p->typeChar());
                if (p->color() == sideToMove) {
                    score += val;
                }
                else {
                    score -= val;
                }
            }
        }

        return score;
    }

    // Basic minimax search without alpha-beta pruning.
    int minimax(int depth, bool maximizingPlayer, Color side) {
        if (depth == 0) {
            return evaluateBoard(side);
        }

        Color player = maximizingPlayer ? side : opponent(side);
        auto moves = generateLegalMoves(player);

        if (moves.empty()) {
            if (isInCheck(player)) {
                return maximizingPlayer ? -100000 : 100000;
            }
            return 0;
        }

        if (maximizingPlayer) {
            int best = std::numeric_limits<int>::min();

            for (auto& m : moves) {
                Piece* moving = board_[m.from.row][m.from.col];
                Piece* captured = board_[m.to.row][m.to.col];

                // Simulate move.
                board_[m.to.row][m.to.col] = moving;
                board_[m.from.row][m.from.col] = nullptr;

                int val = minimax(depth - 1, false, side);

                // Undo move.
                board_[m.from.row][m.from.col] = moving;
                board_[m.to.row][m.to.col] = captured;

                if (val > best) {
                    best = val;
                }
            }

            return best;
        }
        else {
            int best = std::numeric_limits<int>::max();

            for (auto& m : moves) {
                Piece* moving = board_[m.from.row][m.from.col];
                Piece* captured = board_[m.to.row][m.to.col];

                // Simulate move.
                board_[m.to.row][m.to.col] = moving;
                board_[m.from.row][m.from.col] = nullptr;

                int val = minimax(depth - 1, true, side);

                // Undo move.
                board_[m.from.row][m.from.col] = moving;
                board_[m.to.row][m.to.col] = captured;

                if (val < best) {
                    best = val;
                }
            }

            return best;
        }
    }

    // Chooses the best move for the AI side by running minimax.
    Move findBestMove(Color aiColor, int depth) {
        auto moves = generateLegalMoves(aiColor);
        Move bestMove{};
        int bestScore = std::numeric_limits<int>::min();

        for (auto& m : moves) {
            Piece* moving = board_[m.from.row][m.from.col];
            Piece* captured = board_[m.to.row][m.to.col];

            // Simulate move.
            board_[m.to.row][m.to.col] = moving;
            board_[m.from.row][m.from.col] = nullptr;

            int score = minimax(depth - 1, false, aiColor);

            // Undo move.
            board_[m.from.row][m.from.col] = moving;
            board_[m.to.row][m.to.col] = captured;

            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
        }

        return bestMove;
    }

    // -------------------------------------------------------------------------
    // Utility helpers
    // -------------------------------------------------------------------------

    static bool inBounds(const Position& pos) {
        return pos.row >= 0 && pos.row < 8 &&
            pos.col >= 0 && pos.col < 8;
    }

    static Color opponent(Color c) {
        if (c == Color::White) return Color::Black;
        if (c == Color::Black) return Color::White;
        return Color::None;
    }

    static const char* colorName(Color c) {
        switch (c) {
        case Color::White: return "White";
        case Color::Black: return "Black";
        default:           return "None";
        }
    }
};

// -----------------------------------------------------------------------------
// Main menu
// -----------------------------------------------------------------------------

// Shows the main menu and returns which side the AI will play.
//   - Color::None  => Human vs Human
//   - Color::Black => Human vs Computer (computer plays Black)
Color showMenuAndGetAISide() {
    while (true) {
        std::cout << "============================\n";
        std::cout << "      C++ Console Chess     \n";
        std::cout << "          by Iman Eivazi    \n";
        std::cout << "============================\n";
        std::cout << "1) Human vs Human\n";
        std::cout << "2) Human vs Computer (Computer = Black)\n";
        std::cout << "3) Exit\n";
        std::cout << "Select option (1-3): ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            return Color::None;
        }

        if (choice == 1) {
            return Color::None;
        }
        else if (choice == 2) {
            return Color::Black;
        }
        else if (choice == 3) {
            std::cout << "Goodbye!\n";
            return Color::None;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n\n";
        }
    }
}

// -----------------------------------------------------------------------------
// Entry point
// -----------------------------------------------------------------------------

int main() {
    Color aiSide = showMenuAndGetAISide();
    if (!std::cin) {
        return 0;
    }

    Game game(aiSide);
    game.run();

    return 0;
}

// I.E