#include <ncurses.h>
#include <iostream>
#include <array>
#include <list>
#include "snake.h"
using namespace std;

const int FIELD_SIZE_X = 20;
const int FIELD_SIZE_Y = 15;
const int SNAKE_INIT_SIZE = 6;

typedef std::array<std::array<char, FIELD_SIZE_X>, FIELD_SIZE_Y> GameFieldArray;
typedef std::list<SnakeSegment> Snake;

void drawField(const GameFieldArray &field, const Snake &snake);
void initField(GameFieldArray &field, Snake &snake);
Snake initSnake();
void init(GameFieldArray &field, Snake &snake);
SnakeSegment getNextMove(SnakeSegment &head);
bool moveSnake(GameFieldArray &field, Snake &snake);
bool fieldCrash(const GameFieldArray &field, const SnakeSegment &head);
bool selfCrash(const Snake &snake, const SnakeSegment &head);

unsigned int random(unsigned int min, unsigned int max) {
    unsigned int rnd = static_cast<unsigned int>(rand());
    return rnd % (max - min) + min;
}

void addApple(GameFieldArray &field, const Snake &snake);
bool appleFound(GameFieldArray &field, const SnakeSegment &head);

int main()
{
    GameFieldArray gameField;
    Snake snake;

    init(gameField, snake);

    bool exit = false;

    while (!exit) {
        int ch = getch();
        bool crash = false;

        switch ( ch )
        {
        case ERR:
            clear();
            drawField(gameField, snake);
            crash = moveSnake(gameField, snake);
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

        if (crash)
        {
            exit = true;
            mvprintw(FIELD_SIZE_Y + 2, 5, "Oh no! You've crashed! Game over");
        }

        refresh();
    }

    mvprintw(FIELD_SIZE_Y + 3, 5, "Press any key...");
    cbreak();
    getch();

    endwin();                    // Выход из curses-режима. Обязательная команда.
    return 0;
}

void drawField(const GameFieldArray &field, const Snake &snake) {
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

void addApple(GameFieldArray &field, const Snake &snake)
{
    bool placeFound = false;
    Point apple;

    while (!placeFound) {
        placeFound = true;

        apple = Point(random(2, FIELD_SIZE_X - 2), random(FIELD_SIZE_Y / 2, FIELD_SIZE_Y - 1));

        for (auto &segm : snake) {
            if (segm == apple)
            {
                placeFound = false;
                break;
            }
        }
    }

    field[apple.y][apple.x] = '@';
}

void initField(GameFieldArray &field, Snake &snake)
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

    addApple(field, snake);
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

void init(GameFieldArray &field, Snake &snake)
{
    snake = initSnake();

    initField(field, snake);

    initscr();                   // Переход в curses-режим
    keypad(stdscr, true);   //Включаем режим чтения функциональных клавиш

    noecho();
    halfdelay(5);

    curs_set(0);
}

bool fieldCrash(const GameFieldArray &field, const SnakeSegment &head)
{
    if (field[head.y][head.x] == '*')
        return true;
    return false;
}

bool selfCrash(const Snake &snake, const SnakeSegment &head)
{
    for (auto &seg : snake) {
        if (head == seg)
            return true;
    }

    return false;
}

bool appleFound(GameFieldArray &field, const SnakeSegment &head)
{
    if (field[head.y][head.x] == '@')
    {
        field[head.y][head.x] = ' ';
        return true;
    }
    return false;
}

bool moveSnake(GameFieldArray &field, Snake &snake)
{
    auto &back = snake.back();
    snake.pop_back();

    auto &head = snake.front();
    auto nextMove = getNextMove(head);

    if (fieldCrash(field, nextMove) || selfCrash(snake, nextMove))
        return true;

    if (appleFound(field, nextMove))
    {
        snake.push_back(back);
        addApple(field, snake);
    }

    snake.push_front(nextMove);

    return false;
}

SnakeSegment getNextMove(SnakeSegment &head)
{
    unsigned int x = head.x + static_cast<unsigned int>(head.dirX);
    unsigned int y = head.y + static_cast<unsigned int>(head.dirY);
    return SnakeSegment(x, y, head.dirX, head.dirY);
}
