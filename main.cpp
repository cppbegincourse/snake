/**
* Simple console Snake game.
* Used chars:
* '#' - wall
* '*' - snake
* ' ' - empty cell
* '@' - apple
*/

#ifdef _WIN32
#define PDC_DLL_BUILD
#include "curses.h"
#else
#include <ncurses.h>
#endif
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "snake.h"

const int FIELD_SIZE_X = 20;
const int FIELD_SIZE_Y = 15;
const int SNAKE_INIT_SIZE = 6;

const char FIELD_CHAR_WALL = '#';
const char FIELD_CHAR_APPLE = '@';
const char FIELD_CHAR_SNAKE = '*';
const char FIELD_CHAR_EMPTY = ' ';

typedef std::array<std::array<char, FIELD_SIZE_X>, FIELD_SIZE_Y> GameFieldArray;
typedef std::list<SnakeSegment> Snake;

GameFieldArray gameField;
Snake snake;
bool exitGame = false;

void init();
void initCurses();
void initField();
void initSnake();

void update();

void endCurses();

void drawField();

char getFieldChar(const Point &p) { return gameField[p.y][p.x]; };
void setFieldChar(const Point &p, const char value) { gameField[p.y][p.x] = value; };
void setFieldChar(const int x, const int y, const char value) { gameField[y][x] = value; };

bool isWall(const Point &p) { return getFieldChar(p) == FIELD_CHAR_WALL; }
bool isSnake(const Point &p) { return getFieldChar(p) == FIELD_CHAR_SNAKE; }
bool isApple(const Point &p) { return getFieldChar(p) == FIELD_CHAR_APPLE; }

bool checkFieldCrash();
bool checkSelfCrash();

bool moveSnake();
SnakeSegment getNextMove();

void setSnakeDirection(DirectionX dirX, DirectionY dirY);

void addApple();
unsigned int random(unsigned int min, unsigned int max);

void drawString(const int x, const int y, const char* str) { mvprintw(y, x, str); };
void drawChar(const int x, const int y, const char ch) { mvaddch(y, x, ch); };

void drawMessage(const char* str) { drawString(5, FIELD_SIZE_Y + 2, str); }

void reactToInput(int key);
bool checkCrash();
void loadLevel(std::string levelFile);

int main()
{
    init();

    refresh();

    update();

    endCurses();

	return 0;
}

void update()
{
    while (!exitGame)
    {
        int ch = getch();

        reactToInput(ch);

        drawField();

        moveSnake();

        if (checkCrash())
        {
            exitGame = true;
            drawMessage("Oh no! You've crashed! Game over");
        }

        refresh();
    }
}

void loadLevel(std::string levelFile) {
    std::ifstream lvlFile;
    lvlFile.open(levelFile);
    std::string line;
    int cnt = 0;
    while (getline(lvlFile, line)) {
        drawString(2, FIELD_SIZE_Y + 5 + cnt, line.c_str());
        cnt++;
    }
    lvlFile.close();
}

bool checkCrash()
{
    return checkFieldCrash() || checkSelfCrash();
}

void reactToInput(int key)
{
    switch (key)
    {
    case ERR: // Returned by curses if there were no user input
        break;
    case 'q':
        exitGame = true;
        drawMessage("Ok, exit game. See you next time!");
        refresh();
        break;
    case KEY_UP:
        setSnakeDirection(DirectionX::NONE, DirectionY::UP);
        break;
    case KEY_DOWN:
        setSnakeDirection(DirectionX::NONE, DirectionY::DOWN);
        break;
    case KEY_LEFT:
        setSnakeDirection(DirectionX::LEFT, DirectionY::NONE);
        break;
    case KEY_RIGHT:
        setSnakeDirection(DirectionX::RIGHT, DirectionY::NONE);
        break;
    default:
        break;
    }
}

void setSnakeDirection(DirectionX dirX, DirectionY dirY)
{
    SnakeSegment &head = snake.front();
    head.dirX = dirX;
    head.dirY = dirY;
}

void endCurses()
{
    cbreak();
    getch();

    endwin();                    // Turn off curses-mode. Mandatory!
}

bool checkCollisionWithSnake(const Point &p)
{
    for (auto &segm : snake)
    {
        if (segm == p)
        {
            return true;
        }
    }

    return false;
}

Point getRandomFieldPoint() { return Point(random(2, FIELD_SIZE_X - 2), random(FIELD_SIZE_Y / 2, FIELD_SIZE_Y - 2)); }

void addApple()
{
	Point apple;

    do
	{
        apple = getRandomFieldPoint();
    } while (checkCollisionWithSnake(apple));

    setFieldChar(apple, FIELD_CHAR_APPLE);
}

bool checkAndEatApple(const SnakeSegment &head)
{
    if (isApple(head))
	{
        setFieldChar(head, FIELD_CHAR_EMPTY);
		return true;
	}
	return false;
}

bool checkFieldCrash()
{
    SnakeSegment &head = snake.front();
    if (isWall(head))
		return true;
	return false;
}

bool checkSelfCrash()
{
    auto &head = snake.front();
    for (auto iter = (++snake.begin()); iter != snake.end(); iter++)
    {
        if (head == *iter)
			return true;
	}

	return false;
}


bool moveSnake()
{
	auto &back = snake.back();
	snake.pop_back();

    auto nextMove = getNextMove();

    if (checkAndEatApple(nextMove))
    {
		snake.push_back(back);
        addApple();
	}

	snake.push_front(nextMove);

	return false;
}

SnakeSegment getNextMove()
{
    auto &head = snake.front();
	unsigned int x = head.x + static_cast<unsigned int>(head.dirX);
	unsigned int y = head.y + static_cast<unsigned int>(head.dirY);
	return SnakeSegment(x, y, head.dirX, head.dirY);
}

void initSnake()
{
    snake.clear();
	SnakeSegment snakeHead((FIELD_SIZE_X - SNAKE_INIT_SIZE) / 2, FIELD_SIZE_Y / 2, DirectionX::LEFT, DirectionY::NONE);
	snake.push_front(snakeHead);

	SnakeSegment *prevSegment = &snakeHead;
    for (int i = 1; i < SNAKE_INIT_SIZE; ++i)
    {
		SnakeSegment newSegment(prevSegment->x + 1, prevSegment->y, DirectionX::LEFT, DirectionY::NONE);
		snake.push_back(newSegment);

		prevSegment = &newSegment;
	}
}

void drawField()
{
    GameFieldArray &field = gameField;
    clear();

    // Draw field itself
    for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_Y; ++i)
    {
		const char* fieldStr = field[i].data();
        drawString(0, i ,fieldStr);
	}

	//Draw snake
    for (auto &snakeSeg : snake)
    {
        drawChar(snakeSeg.x, snakeSeg.y, FIELD_CHAR_SNAKE);
	}
}

void initCurses()
{
    initscr();              // Go to curses-mode
    keypad(stdscr, true);   // Turn on function keys reading

    noecho();
    halfdelay(5);

    curs_set(0);
}

void init()
{
    initSnake();
    initField();

    initCurses();
}

void initField()
{
    GameFieldArray &field = gameField;
    for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_Y; ++i)
    {
        setFieldChar(FIELD_SIZE_X - 1, i, '\0');
	}

    for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_X - 1; ++i)
    {
        setFieldChar(i, 0, FIELD_CHAR_WALL);
        setFieldChar(i, FIELD_SIZE_Y - 1, FIELD_CHAR_WALL);
	}

    for (GameFieldArray::size_type i = 1; i < FIELD_SIZE_Y - 1; ++i)
    {
        setFieldChar(0, i, FIELD_CHAR_WALL);
        setFieldChar(FIELD_SIZE_X - 2, i, FIELD_CHAR_WALL);

        for (GameFieldArray::size_type j = 1; j < FIELD_SIZE_X - 2; ++j)
        {
            setFieldChar(j, i, FIELD_CHAR_EMPTY);
        }
	}

    addApple();
}

unsigned int random(unsigned int min, unsigned int max)
{
    unsigned int rnd = static_cast<unsigned int>(rand());
    return rnd % (max - min) + min;
}
