#include "Breakout.h"

#include <iostream>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <vector>

/* public */
Point::Point(int16_t x, int16_t y) {
    this->x = x;
    this->y = y;
}

bool Point::operator ==(Point position) const {
    return this->x == position.x && this->y == position.y;
}
void Point::operator +=(Point position) {
    this->x += position.x;
    this->y += position.y;
}
void Point::operator -=(Point position) {
    this->x -= position.x;
    this->y -= position.y;
}

/* public */
Line::Line(Point A, Point B) {
    this->A = A;
    this->B = B;
}

bool Line::contains_point(const Point& point) const { /* A is the point */
    int16_t a = this->B.y - this->A.y;
    int16_t b = this->B.x - this->A.x;

    if (point == this->A || point == this->B) {
        return true;
    } else if (b == 0) {
        if (point.y >= this->B.y && point.y <= this->A.y && point.x - this->A.x == 0) {
            return true;
        } else {
            return false;
        }
    }

    return
            (point.y - this->A.y) / double(point.x - this->A.x) == a / (double)b &&
            point.x >= this->A.x && point.x <= this->B.x &&
            point.y >= this->A.y && point.y <= this->B.y;
}

bool Line::intersects_with(const Line& line) const {
    if (line.contains_point(this->A) || line.contains_point(this->B))
        return true;

    int32_t determinant = (this->A.x - this->B.x) * (line.A.y - line.B.y) - (this->A.y - this->B.y) * (line.A.x - line.B.x);

    // The lines are parallel.
    if (determinant == 0) return false;

    // Get the x and y
    int16_t pre = this->A.x * this->B.y - this->A.y * this->B.x;
    int16_t post = line.A.x * line.B.y - line.A.y * line.A.x;
    int16_t x = (pre * (line.A.x - line.B.x) - (this->A.x - this->B.x) * post) / determinant;
    int16_t y = (pre * (line.A.y - line.B.y) - (this->A.y - this->B.y) * post) / determinant;

    // Check if the x and y coordinates are within both lines
    if ( x < std::min(this->A.x, this->B.x) || x > std::max(this->A.x, this->B.x) ||
         x < std::min(line.A.x, line.B.x) || x > std::max(line.A.x, line.B.x) ) return false;
    if ( y < std::min(this->A.y, this->B.y) || y > std::max(this->A.y, this->B.y) ||
         y < std::min(line.A.y, line.B.y) || y > std::max(line.A.y, line.B.y) ) return false;


    return true;
}


/* private */
Direction Breakout::get_direction(const Point& delta) {
    if (delta.x < 0 && delta.y < 0)
        return Direction::TOP_LEFT;
    else if (delta.x == 0 && delta.y < 0)
        return Direction::TOP;
    else if (delta.x > 0 && delta.y < 0)
        return Direction::TOP_RIGHT;
    else if (delta.x > 0 && delta.y == 0)
        return Direction::RIGHT;
    else if (delta.x > 0 && delta.y > 0)
        return Direction::BOTTOM_RIGHT;
    else if (delta.x == 0 && delta.y > 0)
        return Direction::BOTTOM;
    else if (delta.x < 0 && delta.y > 0)
        return Direction::BOTTOM_LEFT;
    else if (delta.x < 0 && delta.y == 0)
        return Direction::LEFT;
}

void Breakout::to_left() {
    this->racquet.x--;
}

void Breakout::to_right() {
    this->racquet.x++;
}

void Breakout::move() {
    Point next_ball(this->ball.x, this->ball.y);
    next_ball += this->delta_step;

    Line top_wall(
            Point(-1, -1),
            Point(this->width, -1)
    );
    Line left_wall(
            Point(-1, -1),
            Point(-1, this->height)
    );
    Line right_wall(
            Point(this->width, -1),
            Point(this->width, this->height)
    );
    Line racquet_line(
            Point(this->racquet.x - this->racquet_length / 2 - 1, this->racquet.y),
            Point(this->racquet.x + this->racquet_length / 2, this->racquet.y)
    );
    Line ball_trajectory(this->ball,next_ball);

    if (ball_trajectory.intersects_with(racquet_line)) {
        this->delta_step.x = this->ball.x - this->racquet.x;
        this->delta_step.y = -this->delta_step.y;
    } else if (this->ball.y >= this->height) {
        this->game_over = true;
    } else if (ball_trajectory.intersects_with(top_wall)) {
        this->delta_step.y = -this->delta_step.y;
    } else if (ball_trajectory.intersects_with(left_wall) || ball_trajectory.intersects_with(right_wall)) {
        this->delta_step.x = -this->delta_step.x;
    } else {
        for (int16_t i = 0; i < this->height; ++i) {
            for (int16_t j = 0; j < this->width; ++j) {
                if (this->board[i][j] == this->BORDER || this->board[i][j] == this->BRICK) {
                    if (ball_trajectory.A.x == 20 && ball_trajectory.A.y == 5 && j == 20 && i == 5) {
                        std::cout << "";
                    }
                    if (ball_trajectory.contains_point(Point(j, i))) {
                        Direction ball_direction = Breakout::get_direction(this->delta_step);

                        switch (ball_direction) {
                            case TOP:
                            case BOTTOM:
                                this->delta_step.y = -this->delta_step.y;
                                break;
                            case LEFT:
                            case RIGHT:
                                this->delta_step.x = -this->delta_step.x;
                                break;
                            case Direction::TOP_LEFT:
                                if (this->board[i + 1][j] == this->BORDER || this->board[i + 1][j] == this->BRICK) {
                                    this->delta_step.x = -this->delta_step.x;
                                } else if (this->board[i][j + 1] == this->BORDER || this->board[i][j + 1] == this->BRICK) {
                                    this->delta_step.y = -this->delta_step.y;
                                }
                                break;
                            case Direction::TOP_RIGHT:
                                if (this->board[i + 1][j] == this->BORDER || this->board[i + 1][j] == this->BRICK) {
                                    this->delta_step.x = -this->delta_step.x;
                                } else if (this->board[i][j - 1] == this->BORDER || this->board[i][j - 1] == this->BRICK) {
                                    this->delta_step.y = -this->delta_step.y;
                                }
                                break;
                            case Direction::BOTTOM_RIGHT:
                                if (this->board[i - 1][j] == this->BORDER || this->board[i - 1][j] == this->BRICK) {
                                    this->delta_step.x = -this->delta_step.x;
                                } else if (this->board[i][j - 1] == this->BORDER || this->board[i][j - 1] == this->BRICK) {
                                    this->delta_step.y = -this->delta_step.y;
                                }
                                break;
                            case Direction::BOTTOM_LEFT:
                                if (this->board[i - 1][j] == this->BORDER || this->board[i - 1][j] == this->BRICK) {
                                    this->delta_step.x = -this->delta_step.x;
                                } else if (this->board[i][j + 1] == this->BORDER || this->board[i][j + 1] == this->BRICK) {
                                    this->delta_step.y = -this->delta_step.y;
                                }
                                break;
                        }

                        if (this->board[i][j] == this->BRICK) {
                            this->board[i][j] = this->SPACE;
                            this->score++;
                        }
                    }
                }
            }
        }
    }

    this->ball += this->delta_step;
}

void Breakout::update_ui() const {
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

void Breakout::listen_key_press() {
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

/* public */
Breakout::Breakout(uint16_t width, uint16_t height) {
    this->width = width * this->aspect_ratio;
    this->height = height;

    this->racquet.x = this->width / 2;
    this->racquet.y = this->height - 1;

    this->ball.x = this->width / 2;
    this->ball.y = this->height - 2;
}

void Breakout::set_board(const std::vector<std::vector<char>> &source) {
    this->board = std::vector<std::vector<char>>(this->height, std::vector<char>(this->width));
    for (int i = 0; i < source.size(); ++i) {
        for (int j = 0; j < source[i].size(); ++j) {
            for (int k = 0; k < this->aspect_ratio; ++k) {
                this->board[i][this->aspect_ratio * j + k] = source[i][j];
            }
        }
    }
}

void Breakout::start() {
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

void Breakout::end() {
    this->game_over = true;
}

Breakout::~Breakout() {
    endwin();
}
