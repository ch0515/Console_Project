#pragma once

#include "main.h"

//--------------------------

/// SCREEN SIZE

#define SCREEN_WIDTH 150
#define SCREEN_HEIGHT 30

//--------------------------

/// ANIMATION LIST

#define DIRECT_LEFT 0
#define DIRECT_RIGHT 1

#define LEFT_WALK_1 0
#define LEFT_WALK_2 1
#define LEFT_ATTACK 2
#define LEFT_JUMP 3
#define RIGHT_WALK_1 4
#define RIGHT_WALK_2 5
#define RIGHT_ATTACK 6
#define RIGHT_JUMP 7

#define CARROT_UP_1 0
#define CARROT_UP_2 1
#define CARROT_UP_3 2
#define CARROT_UP_4 3
#define CARROT_UP_5 4

#define CARROT_DOWN_1 5
#define CARROT_DOWN_2 6
#define CARROT_DOWN_3 7
#define CARROT_DOWN_4 8
#define CARROT_DOWN_5 9

//--------------------------

/// COLLISION

#define COLLCHECK_DOWN 0
#define COLLCHECK_UP 1
#define COLLCHECK_X 2
#define COLLCHECK_NO 3

//--------------------------

/// CARROT SETTING

/// 당근 출력 개수
#define CARROT_COUNT 7

#define NONE 0
#define EXE 1
#define FREE 2

//--------------------------

/// CARROT POS Y

#define FLOOR_0F 22
#define FLOOR_1F 16
#define FLOOR_2F 12
#define FLOOR_3F 11
#define FLOOR_4F 7
#define FLOOR_5F 4

//--------------------------

/// UI SETTING

#define UI_POS_X 54
#define UI_POS_Y 1
#define UI_SIZE_X 44
#define UI_SIZE_Y 2

#define MAX_NAME_LEN 11

/// ★★★★★★★★★
/// 게임 종료 시간
#define GAME_TIME_LIMIT 60 //60

//--------------------------

/// UI DRAW COLOR MACRO

#define SET_GREEN SetConsoleTextAttribute(hConsole, 10);
#define SET_YELLOW SetConsoleTextAttribute(hConsole, 14);
#define SET_WHITE SetConsoleTextAttribute(hConsole, 15);
#define SET_BLUE SetConsoleTextAttribute(hConsole, 11);

//--------------------------

/// STRUCT

typedef struct _Player {

	int score;

	//player 좌표값
	short x, y;
	short sizeX, sizeY;
	int jumpPower;

	int tick;
	int attackTick;
	double jump;
	BOOL isJump;
	BOOL isAttack;

	//player sprite
	int animation;
	int direct;
	char sprite[8][3][8];

} Player;

typedef struct _CarrotPos {
	short x, y;
} CarrotPos;

typedef struct _Carrot {

	//carrot 좌표값
	short x, y;
	short sizeX, sizeY;

	int animation;
	int tick;
	int holdingTime;
	int generateTick;

	BOOL isGenerated;
	BOOL isDamaged;
	BOOL dieAnimation;
	BOOL disappearAnimation;

	//sprite[9][4][7]
	char(*sprite)[4][7];

} Carrot;

typedef struct _Rank
{
	char name[MAX_NAME_LEN + 1];
	int score;
} Rank;

//--------------------------

/// FUNCTION

void GameReset();

void Render();
void Present();
void BufferReset();

void CharacterDraw(int,int);
void CharacterJump();
void CharacterMove();

void CarrotManagement();
void CarrotCreate(Carrot*);
Carrot* CarrotGenerator();
void CarrotDraw(Carrot*, int, int);
Carrot* CarrotInit(Carrot*, short, short, int, int);

int CollisionCheck(int, int, int, int, int);

void MapDraw();
void MapDie();

void ScoreAndTimeUI();
void UIDraw();

void TimeOverDraw();
void ShowScore(HANDLE);
void RemoveSpaces(char*);
void RankingRegister(HANDLE);
void ScreenCloseOpen();

void TitleDraw();
void RankingDraw();
void RankingRoad(Rank*);
void BubbleSort(Rank[], int);

int MenuDraw();
void StartMenuDraw(HANDLE);
void RankingMenuDraw(HANDLE);
void ExitMenuDraw(HANDLE);
