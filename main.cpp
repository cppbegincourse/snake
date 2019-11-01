#ifdef _WIN32
#define PDC_DLL_BUILD
#include "curses.h"
#else
#include <ncurses.h>
#endif
#include <array>
#include <iostream>
#include <list>
#include "snake.h"

const int FIELD_SIZE_X = 20;
const int FIELD_SIZE_Y = 15;
const int SNAKE_INIT_SIZE = 6;

typedef std::array<std::array<char, FIELD_SIZE_X>, FIELD_SIZE_Y> GameFieldArray;
typedef std::list<SnakeSegment> Snake;

void init(GameFieldArray &field, Snake &snake);
void initField(GameFieldArray &field, Snake &snake);
Snake initSnake();

void drawField(GameFieldArray &field, Snake &snake);
bool moveSnake(GameFieldArray &field, Snake &snake);
SnakeSegment getNextMove(SnakeSegment &head);

void addApple(GameFieldArray &field, const Snake &snake);

unsigned int random(unsigned int min, unsigned int max) {
	unsigned int rnd = static_cast<unsigned int>(rand());
	return rnd % (max - min) + min;
}

int main()
{
	GameFieldArray gameField;
	Snake snake;

	init(gameField, snake);

	bool exit = false;

	while (!exit) {
		int ch = getch();
		bool crash = false;

		switch (ch)
		{
		case ERR:
			clear();
			drawField(gameField, snake);
			crash = moveSnake(gameField, snake);
			break;
		case 'q':
			exit = true;
			break;
		case KEY_UP:
		{
			SnakeSegment &head = snake.front();
			head.dirX = DirectionX::NONE;
			head.dirY = DirectionY::UP;
			break;
		}
		case KEY_DOWN:
		{
			SnakeSegment &head = snake.front();
			head.dirX = DirectionX::NONE;
			head.dirY = DirectionY::DOWN;
			break;
		}
		case KEY_LEFT:
		{
			SnakeSegment &head = snake.front();
			head.dirX = DirectionX::LEFT;
			head.dirY = DirectionY::NONE;
			break;
		}
		case KEY_RIGHT:
		{
			SnakeSegment &head = snake.front();
			head.dirX = DirectionX::RIGHT;
			head.dirY = DirectionY::NONE;
			break;
		}
		default:
			break;
		}

		if (crash)
		{
			exit = true;
			mvprintw(FIELD_SIZE_Y + 2, 5, "Oh no! You've crashed! Game over");
		}

		refresh();
	}

	cbreak();
	getch();

	endwin();                    // Выход из curses-режима. Обязательная команда.

	system("pause");

	return 0;
}

void addApple(GameFieldArray &field, const Snake &snake)
{
	bool placeFound = false;
	Point apple;

	while (!placeFound)
	{
		placeFound = true;
		apple = Point(random(2, FIELD_SIZE_X - 2), random(FIELD_SIZE_Y / 2, FIELD_SIZE_Y - 2));

		for (auto &segm : snake)
		{
			if (segm == apple)
			{
				placeFound = false;
				break;
			}
		}
	}

	field[apple.y][apple.x] = '@';
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

bool fieldCrash(GameFieldArray &field, SnakeSegment &head)
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


bool moveSnake(GameFieldArray &field, Snake &snake)
{
	auto &back = snake.back();
	snake.pop_back();

	auto &head = snake.front();
	auto nextMove = getNextMove(head);

	if (fieldCrash(field, nextMove) || selfCrash(snake, nextMove))
		return true;

	if (appleFound(field, nextMove)) {
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

Snake initSnake()
{
	Snake snake;

	SnakeSegment snakeHead((FIELD_SIZE_X - SNAKE_INIT_SIZE) / 2, FIELD_SIZE_Y / 2, DirectionX::LEFT, DirectionY::NONE);
	snake.push_front(snakeHead);

	SnakeSegment *prevSegment = &snakeHead;
	for (int i = 1; i < SNAKE_INIT_SIZE; ++i) {
		SnakeSegment newSegment(prevSegment->x + 1, prevSegment->y, DirectionX::LEFT, DirectionY::NONE);
		snake.push_back(newSegment);

		prevSegment = &newSegment;
	}

	return snake;
}

void drawField(GameFieldArray &field, Snake &snake)
{
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

void init(GameFieldArray &field, Snake &snake)
{
	snake = initSnake();
	initField(field, snake);

	initscr();              // Переход в curses-режим
	keypad(stdscr, true);   //Включаем режим чтения функциональных клавиш

	noecho();
	halfdelay(5);

	curs_set(0);
}

void initField(GameFieldArray &field, Snake &snake)
{
	for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_Y; ++i) {
		field[i][FIELD_SIZE_X - 1] = '\0';
	}

	for (GameFieldArray::size_type i = 0; i < FIELD_SIZE_X - 1; ++i) {
		field[0][i] = '*';
		field[FIELD_SIZE_Y - 1][i] = '*';
	}

	for (GameFieldArray::size_type i = 1; i < FIELD_SIZE_Y - 1; ++i) {
		field[i][0] = '*';
		field[i][FIELD_SIZE_X - 2] = '*';

		for (GameFieldArray::size_type j = 1; j < FIELD_SIZE_X - 2; ++j) {
			field[i][j] = ' ';
		}
	}

	addApple(field, snake);
}