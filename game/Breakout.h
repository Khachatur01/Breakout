#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <iostream>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <vector>

enum Direction {
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT
};

class Point {
public:
    int16_t x;
    int16_t y;
    Point(int16_t x, int16_t y);

    bool operator ==(Point position) const;
    void operator +=(Point position);
    void operator -=(Point position);
};
class Line {
public:
    Point A = Point(0, 0);
    Point B = Point(0, 0);
    Line(Point A, Point B);

    [[nodiscard]] bool contains_point(const Point& point) const;

    [[nodiscard]] bool intersects_with(const Line& line) const;
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
    Point delta_step = Point(0, -1);
    uint8_t aspect_ratio = 21 / 9; /* 21 and 9 are height and width of console character */
    uint32_t delay = 200 * 1000;

    static Direction get_direction(const Point& delta);

    void to_left();
    void to_right();

    void move();

    void update_ui() const;

    void listen_key_press();

public:
    const char BALL = '@';
    const char RACQUET = '=';
    const char BRICK = '*';
    const char SPACE = ' ';
    const char BORDER = '#';

    Breakout(uint16_t width, uint16_t height);

    void set_board(const std::vector<std::vector<char>> &source);

    void start();
    void end();

    ~Breakout();
};

#endif //BREAKOUT_H
