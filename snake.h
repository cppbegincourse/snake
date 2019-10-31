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
    unsigned int x;
    unsigned int y;

    Point() : x(0), y(0) {}
    Point(unsigned int x, unsigned int y) : x(x), y(y) {}
};

class SnakeSegment : public Point {
public:
    SnakeSegment(unsigned int _x, unsigned int _y) : Point(_x, _y), dirX(DirectionX::NONE), dirY(DirectionY::NONE) {}
    SnakeSegment(unsigned int _x, unsigned int _y, DirectionX _dirX, DirectionY _dirY) : Point(_x, _y), dirX(_dirX), dirY(_dirY) {}
    DirectionX dirX;
    DirectionY dirY;

    bool operator== (const SnakeSegment &segm) const { return x == segm.x && y == segm.y; }
    bool operator== (const Point &point) const { return x == point.x && y == point.y; }
};
