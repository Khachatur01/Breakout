#include "game/Breakout.h"

int main() {
    Breakout breakout(20, 20);

    std::vector<std::vector<char>> board(20, std::vector<char>(20, breakout.SPACE));

    for (int j = 5; j < 15; ++j) {
        board[5][j] = breakout.BORDER;
        if (j == 5 || j == 14) {
            for (int i = 5; i < 10; ++i) {
                board[i][j] = breakout.BRICK;
            }
        }
    }
    breakout.set_board(board);

    breakout.start();

    return 0;
}
