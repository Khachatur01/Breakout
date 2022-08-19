#include <iostream>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <vector>

class Point {
public:
    int16_t x;
    int16_t y;
    Point(int16_t x, int16_t y) {
        this->x = x;
        this->y = y;
    }

    bool operator ==(Point position) const {
        return this->x == position.x && this->y == position.y;
    }
    void operator +=(Point position) {
        this->x += position.x;
        this->y += position.y;
    }
    void operator -=(Point position) {
        this->x -= position.x;
        this->y -= position.y;
    }
};
class Line {
public:
    Point A = Point(0, 0);
    Point B = Point(0, 0);
    Line(Point A, Point B) {
        this->A = A;
        this->B = B;
    }

    bool contains_point(Point point) { /* A is the point */
        int16_t a = this->B.y - this->A.y;
        int16_t b = this->B.x - this->A.x;

        return
            (point.y - this->A.y) / double(point.x - this->A.x) == a / (double)b &&
            point.x >= this->A.x && point.x <= this->B.x &&
            point.y >= this->A.y && point.y <= this->B.y;
    }

    bool intersects_with(Line line) {
        if (line.contains_point(this->A) || line.contains_point(this->B))
            return true;

        int32_t determinant = (this->A.x - this->B.x) * (line.A.y - line.B.y) - (this->A.y - this->B.y) * (line.A.x - line.B.x);

        // Get the x and y
        int16_t pre = (this->A.x * this->B.y - this->A.y * this->B.x), post = int16_t(line.A.x * line.B.y - line.A.y * line.A.x);
        int16_t x = (pre * (line.A.x - line.B.x) - (this->A.x - this->B.x) * post) / determinant;
        int16_t y = (pre * (line.A.y - line.B.y) - (this->A.y - this->B.y) * post) / determinant;

        // Check if the x and y coordinates are within both lines
        if ( x < std::min(this->A.x, this->B.x) || x > std::max(this->A.x, this->B.x) ||
             x < std::min(line.A.x, line.B.x) || x > std::max(line.A.x, line.B.x) ) return false;
        if ( y < std::min(this->A.y, this->B.y) || y > std::max(this->A.y, this->B.y) ||
             y < std::min(line.A.y, line.B.y) || y > std::max(line.A.y, line.B.y) ) return false;
        // The lines are parallel.
        if (determinant == 0) return false;

        return true;
    }
};

class Breakout {
private:
    bool game_over = false;
    bool paused = false;
    uint16_t score = 0;
    uint16_t width;
    uint16_t height;
    std::vector<std::vector<char>> board;
    Point ball = Point(0, 0);
    Point racquet = Point(0, 0);
    uint8_t racquet_length = 5;
    Point delta_step = Point(1, -1);
    uint8_t aspect_ratio = 21 / 9; /* 21 and 9 are height and width of console character */
    uint32_t delay = 200 * 1000;

    void to_left() {
        this->racquet.x--;
    }
    void to_right() {
        this->racquet.x++;
    }

    void move() {
        Point next_ball(this->ball.x, this->ball.y);
        next_ball += this->delta_step;

        Line racquet_line(
        Point(this->racquet.x - this->racquet_length / 2, this->racquet.y),
        Point(this->racquet.x + this->racquet_length / 2, this->racquet.y)
        );
        Line ball_trajectory(this->ball,next_ball);

        if (ball_trajectory.intersects_with(racquet_line)) {
            this->delta_step.x = this->ball.x - this->racquet.x;
            this->delta_step.y = (int16_t)-this->delta_step.y;
        } else if (this->ball.y < 1) {
            this->delta_step.y = (int16_t)-this->delta_step.y;
        } else if (this->ball.x >= this->width - 1 || this->ball.x <= 0) {
            this->delta_step.x = (int16_t)-this->delta_step.x;
        } else if (this->ball.y >= this->height) {
            this->game_over = true;
        }

        this->ball += this->delta_step;
    }

    void update_ui() const {
        system("clear");
        std::cout << "Score: " << this->score << "\r\n";
        for (uint8_t j = 0; j < width + 2; ++j) {
            std::cout << this->BORDER;
        }
        std::cout << "\r\n";

        for (uint16_t i = 0; i < height; ++i) {
            for (int16_t j = -1; j < width + 1; ++j) {
                if (j == -1 || j == width) {
                    std::cout << Breakout::BORDER;
                } else if (j == this->ball.x && i == this->ball.y) {
                    std::cout << this->BALL;
                } else if (i == this->racquet.y && (j >= this->racquet.x - this->racquet_length / 2 && j <= this->racquet.x + this->racquet_length / 2)) {
                    std::cout << this->RACQUET;
                } else if (this->board[i][j] != Breakout::SPACE) {
                    std::cout << this->board[i][j];
                } else {
                    std::cout << Breakout::SPACE;
                }
            }
            std::cout << "\r\n";
        }

        for (uint8_t j = 0; j < width + 2; ++j) {
            std::cout << Breakout::BORDER;
        }
        std::cout << "\r\n";
    }

    void listen_key_press() {
        while (!this->game_over) {
            switch (getch()) {
                case 'A':
                case 'a':
                    this->to_left();
                    break;
                case 'D':
                case 'd':
                    this->to_right();
                    break;
                case 'P':
                case 'p':
                    this->paused = !this->paused;
                    break;
            }
            refresh();
        }
    }

public:
    const char BALL = '@';
    const char RACQUET = '=';
    const char BRICK = '*';
    const char SPACE = ' ';
    const char BORDER = '#';

    Breakout(uint16_t width, uint16_t height) {
        this->width = width * this->aspect_ratio;
        this->height = height;

        this->racquet.x = this->width / 2;
        this->racquet.y = this->height - 1;

        this->ball.x = this->width / 2;
        this->ball.y = this->height - 2;
    }

    void set_board(const std::vector<std::vector<char>> &source) {
        this->board = std::vector<std::vector<char>>(this->height, std::vector<char>(this->width));
        for (int i = 0; i < source.size(); ++i) {
            for (int j = 0; j < source[i].size(); ++j) {
                for (int k = 0; k < this->aspect_ratio; ++k) {
                    this->board[i][this->aspect_ratio * j + k] = source[i][j];
                }
            }
        }
    }

    void start() {
        initscr();
        noecho();
        std::thread key_press_thread(&Breakout::listen_key_press, this);

        while (!this->game_over) {
            if (!this->paused) {
                this->move();
                this->update_ui();
                usleep(this->delay);
            }
        }

        key_press_thread.detach();
    }
    void end() {
        this->game_over = true;
    }

    ~Breakout() {
        endwin();
    }
};