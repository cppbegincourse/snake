#include <ncurses.h>
#include <iostream>
#include <array>
#include <list>
#include "snake.h"
using namespace std;

const int FIELD_SIZE_X = 70;
const int FIELD_SIZE_Y = 40;
const int SNAKE_INIT_SIZE = 6;

typedef std::array<std::array<char, FIELD_SIZE_X>, FIELD_SIZE_Y> GameFieldArray;
typedef std::list<SnakeSegment> Snake;


void drawGF(const GameFieldArray &field, const Snake &snake);
void initGF(GameFieldArray &field);

Snake initSnake();
void init(GameFieldArray &gf, Snake &snake);

SnakeSegment getNextMove(SnakeSegment &head)
{
    int x = head.x + (int)head.dirX;
    int y = head.y + (int)head.dirY;
    return SnakeSegment(x, y, head.dirX, head.dirY);
}

void moveSnake(const GameFieldArray &gf, Snake &snake);

int main()
{
    GameFieldArray gameField;
    Snake snake;

    init(gameField, snake);

    bool exit = false;

    while (!exit) {
        int ch = getch();

        switch ( ch )
        {
        case ERR:
            clear();
            drawGF(gameField, snake);
            moveSnake(gameField, snake);
            break;
        case KEY_F(2): //Выходим из программы, если была нажата F2
        case 'q': //Или 'q'
            exit = true;
            break;
        case KEY_UP:
        {
            SnakeSegment &head = *snake.begin();
            head.dirX = DirectionX::NONE;
            head.dirY = DirectionY::UP;
            break;
        }
        case KEY_DOWN:
        {
            SnakeSegment &head = *snake.begin();
            head.dirX = DirectionX::NONE;
            head.dirY = DirectionY::DOWN;
            break;
        }
        case KEY_LEFT:
        {
            SnakeSegment &head = *snake.begin();
            head.dirX = DirectionX::LEFT;
            head.dirY = DirectionY::NONE;
            break;
        }
        case KEY_RIGHT:
        {
            SnakeSegment &head = *snake.begin();
            head.dirX = DirectionX::RIGHT;
            head.dirY = DirectionY::NONE;
            break;
        }
        default:  //Если всё нормально, выводим код нажатой клавиши
            printw("Code of pressed key is %d\n", ch);
            break;
        }

        refresh();
    }


    endwin();                    // Выход из curses-режима. Обязательная команда.
    return 0;
}

void drawGF(const GameFieldArray &field, const Snake &snake) {
    // Draw field itself
    for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_Y; ++i) {
        const char* fieldStr = field[i].data();
        mvprintw(static_cast<int>(i), 0, fieldStr);
    }

    //Draw snake
    for (auto &snakeSeg : snake) {
        mvaddch(snakeSeg.y, snakeSeg.x, '*');
    }
}

void initGF(GameFieldArray &field)
{
    for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_Y - 1; ++i) {
        field[i][FIELD_SIZE_X - 1] = '\0';
    }

    for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_X - 1; ++i) {
        field[0][i] = '*';
        field[FIELD_SIZE_Y - 1][i] = '*';
    }

    for (GameFieldArray::size_type i = 1; i < FIELD_SIZE_Y - 1; ++i) {
        field[i][0] = '*';
        field[i][FIELD_SIZE_X - 2] = '*';

        for (GameFieldArray::size_type j  = 1; j < FIELD_SIZE_X - 2; ++j) {
            field[i][j] = ' ';
        }
    }
}

Snake initSnake()
{
    Snake snake;

    SnakeSegment snakeHead((FIELD_SIZE_X - SNAKE_INIT_SIZE) / 2, FIELD_SIZE_Y / 2);
    snakeHead.dirX = DirectionX::LEFT;
    snakeHead.dirY = DirectionY::NONE;
    snake.push_front(snakeHead);

    SnakeSegment *prevSegment = &snakeHead;
    for (int i = 1; i < SNAKE_INIT_SIZE; ++i) {
        SnakeSegment newSegment(prevSegment->x + 1, prevSegment->y);
        newSegment.dirX = DirectionX::LEFT;
        newSegment.dirY = DirectionY::NONE;
        snake.push_back(newSegment);

        prevSegment = &newSegment;
    }

    return snake;
}

void init(GameFieldArray &gf, Snake &snake)
{
    snake = initSnake();

    initGF(gf);

    initscr();                   // Переход в curses-режим
    keypad(stdscr, true);   //Включаем режим чтения функциональных клавиш

    noecho();
    halfdelay(5);

    curs_set(0);
}

void moveSnake(const GameFieldArray &gf, Snake &snake)
{
    snake.pop_back();

    auto &head = snake.front();
    snake.push_front(getNextMove(head));
}
