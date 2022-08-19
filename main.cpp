#include "game/main.h"

int main() {

    Breakout breakout(20, 20);

    std::vector<std::vector<char>> board(20, std::vector<char>(20, breakout.SPACE));
    board[5][7] = breakout.BORDER;
    board[5][8] = breakout.BRICK;
    board[5][9] = breakout.BRICK;
    board[5][10] = breakout.BRICK;
    board[5][11] = breakout.BRICK;
    board[5][12] = breakout.BRICK;
    board[5][13] = breakout.BORDER;
    breakout.set_board(board);

    breakout.start();

    return 0;
}
