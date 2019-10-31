#pragma once
#include <array>
#include <list>

enum class DirectionY {
    UP = -1,
    NONE = 0,
    DOWN = 1,
};

enum class DirectionX {
    LEFT = -1,
    NONE = 0,
    RIGHT = 1
};


class Point {
public:
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
};

class SnakeSegment : public Point {
public:
    SnakeSegment(int _x, int _y) : Point(_x, _y), dirX(DirectionX::NONE), dirY(DirectionY::NONE) {}
    SnakeSegment(int _x, int _y, DirectionX _dirX, DirectionY _dirY) : Point(_x, _y), dirX(_dirX), dirY(_dirY) {}
    DirectionX dirX;
    DirectionY dirY;
};
