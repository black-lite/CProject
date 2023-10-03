#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <winuser.h>
#include <wincon.h>
#include <math.h>

#define width 65
#define height 25
#define WALL '#'
#define RACKET '@'
#define BALL '*'

typedef struct { int x, y, w; } TRacket;
typedef struct {
	float x, y;
	int ix, iy;
	float alfa;
	float speed;
} TBall;

char field[height][width + 1];
TRacket racket;
TBall ball;

int hitCnt = 0;
int maxHitCnt = 0;

void initRacket()
{
	racket.w = 7;
	racket.x = width / 2 - racket.w / 2;
	racket.y = height - 1;
}

void putRacket() { for (int i = racket.x; i < racket.x + racket.w; i++) field[racket.y][i] = RACKET; }

void moveRacket(int x)
{
	racket.x = x;
	if (racket.x < 1) racket.x = 1;
	if (racket.x + racket.w >= width) racket.x = width - 1 - racket.w;
}

void moveBall(float x, float y)
{
	ball.x = x;
	ball.y = y;
	ball.ix = (int)roundf(ball.x);
	ball.iy = (int)roundf(ball.y);
}

void initBall()
{
	moveBall((float)2, (float)2);
	ball.alfa = -1;
	ball.speed = 0.5;
}

void putBall() { field[ball.iy][ball.ix] = BALL; }

void autoMoveBall()
{
	if (ball.alfa < 0) ball.alfa += M_PI * 2;
	if (ball.alfa > M_PI * 2) ball.alfa -= M_PI * 2;

	TBall bl = ball;
	moveBall(ball.x + cosf(ball.alfa) * ball.speed, ball.y + sinf(ball.alfa) * ball.speed);

	if (field[ball.iy][ball.ix] == WALL || field[ball.iy][ball.ix] == RACKET)
	{
		if (field[ball.iy][ball.ix] == RACKET) hitCnt++;

		if ((ball.ix != bl.ix) && (ball.iy != bl.iy))
		{
			if (field[bl.iy][ball.ix] == field[ball.iy][bl.ix]) bl.alfa = bl.alfa + M_PI;
			else
			{
				if (field[bl.iy][ball.ix] == WALL) bl.alfa = (2 * M_PI - bl.alfa) + M_PI;
				else bl.alfa = 2 * M_PI - bl.alfa;
			}
		}
		else if (ball.iy == bl.iy) bl.alfa = (2 * M_PI - bl.alfa) + M_PI;
		else bl.alfa = 2 * M_PI - bl.alfa;

		ball = bl;
		autoMoveBall();
	}
}

void initField()
{
	// Заполняем 0 строку символом '#' и в конце вставляем символ конца строки
	for (int i = 0; i < width; i++) field[0][i] = WALL;
	//field[0][width] = '\0';

	/* Копируем из 0 width + 1(конец строки) символов в 1 строку */
	strncpy(field[1], field[0], width + 1);

	/* Заполняем 1 строку пробелами начиная с 1 до предпоследнего */
	for (int i = 1; i < width - 1; i++) field[1][i] = ' ';

	/* Копируем из 1 строки width + 1(конец строки) символов в i строку начиная со 2 строки */
	for (int i = 2; i < height; i++) strncpy(field[i], field[1], width + 1);

	//for (int i = 20; i < 50; i++) field[10][i] = WALL;
}

void showFiled()
{
	for (int i = 0; i < height; i++)
	{
		printf("%s",  field[i]);

		if (i == 3) printf("   hit %i   ", hitCnt);
		if (i == 4) printf("   max %i   ", maxHitCnt);

		if (i == 6) printf("   size %llu   ", sizeof(field));

		if (i < height - 1) printf("\n");
	}
}

//typedef struct _COORD {
//	SHORT X;
//	SHORT Y;
//} COORD, *PCOORD;

void setcur(short x, short y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;

	// TODO РАЗОБРАТЬСЯ КАК ЭТО РАБОТАЕТ
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

	// TODO РАЗОБРАТЬСЯ КАК ЭТО РАБОТАЕТ
	// ЭТО ДЕЛАЕТ НЕВИДИМЫМ КУРСОР
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle, &structCursorInfo);
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &structCursorInfo);
}

int main()
{
	int c = 0;
	initRacket();
	initBall();

	BOOL run = FALSE;


	do
	{
		setcur(0, 0);

		if (run) autoMoveBall();
		if (ball.iy > height)
		{
			run = FALSE;
			if (hitCnt > maxHitCnt) maxHitCnt = hitCnt;
			hitCnt = 0;
		}

		initField();
		putRacket();
		putBall();
		showFiled();

		if (GetKeyState('A') < 0) moveRacket(--racket.x);
		if (GetKeyState('D') < 0) moveRacket(++racket.x);
		if (GetKeyState('W') < 0) run = TRUE;

		if (!run)
		{
			moveBall(racket.x + racket.w / 2, racket.y - 1);
		}

		printf("run = %d", run);

//		Sleep(20);

	}
	while (GetKeyState(VK_ESCAPE) >= 0);

	free(field);
	return 0;
}
