#include "draw.h"
#include "input.h"
#include "util.h"
#include "main.h"
#include "game.h"

Player player = { 0, (SCREEN_WIDTH / 2) - 3, SCREEN_HEIGHT - 7, 8, 3, 7, 0, 0, 0.0, FALSE, FALSE, LEFT_WALK_1, DIRECT_LEFT,
	{
		/// 왼쪽
		{"  /)_/) ", " ('-' ) ", " (_(_ o " },		// WALK 1
		{" (\\_(\\ ", " ('-' ) ", " (_ (_ o" },		// WALK 2
		{"  /)_/) ", "@('^' ) ", "  (_(_ o" },		// ATTACK
		{"  /)_/) ", " ('o' o ", " (_(_/ " },		// JUMP

		/// 오른쪽
		{" (\\_(\\ ", " ( '-') ", " o _)_) " },		// WALK 1
		{"  /)_/) ", " ( '-') ", "o _) _) " },		// WALK 2
		{" (\\_(\\ ", " ( '^')@", "o _)_) " },		// ATTACK
		{" (\\_(\\ ", " o 'o') ", "  \\_)_)" }		// JUMP
	}
};

char carrotSprite[10][4][7] = {

	/// 당근 나와용
	{"       " , "       ", "       " , "       "},			// CARROT_UP_1
	{"       " , "       " , "       " , "  \\|/  "},		// CARROT_UP_2
	{"       " , "       " , "  \\|/  " , "/_____\\"},		// CARROT_UP_3
	{"       " , "  \\|/  " , "/_____\\" , "\\%'-'%/"},		// CARROT_UP_4
	{"  \\|/  " , "/_____\\" , "\\%'-'%/", " \\=%%/ "},		// CARROT_UP_5

	/// 당근 들어가용
	{"  \\|/  " , "/_____\\" , "\\%x_x%/" , " \\=%%/ "},	// CARROT_DOWN_1
	{"       " , "  \\|/  " , "/_____\\" , "\\%x_x%/"},		// CARROT_DOWN_2
	{"       " , "       ", "  \\|/  " , "/_____\\"},		// CARROT_DOWN_3
	{"       " , "       ", "       " , "  \\|/  "},		// CARROT_DOWN_4
	{"       " , "       ", "       " , "       "}			// CARROT_DOWN_5

};

char scoreAndTimeUI[2][44] = {
	{"             ** SCORE : 000 **             "},
	{"00:00 |------------------------------ 01:00"}
};

// 초기 맵 문자 저장
char** map;

CHAR_INFO screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
CHAR_INFO backBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
CHAR_INFO* pScreenBuffer = &screenBuffer[0][0];

// 당근 좌표값 중복 방지 변수들
int carrotPosCheck[CARROT_COUNT], index;
BOOL isCreated = TRUE;

int isGenerate = NONE;
Carrot* pCarrot[CARROT_COUNT] = { 0 };
int carrotDieCount = 0;

// 시간제한 관련 변수
extern startTime;
extern ULONGLONG gameLoopStartTime;
ULONGLONG currentTime;
BOOL playbarMove = TRUE;
int elapsedTime;
int secondFront, secondBack, minuteBack;
int playbarPos = 6;
int scoreFirst, scoreSecond, scoreThird;

char name[MAX_NAME_LEN + 1] = {'\0'};
extern gameMode;

/// 게임 초기화
void GameReset()
{
	// 랜덤 시드 초기화
	srand(time(NULL));
	// 당근 동적할당 해제
	if (isGenerate != FREE)
		isGenerate = FREE;
	// 시작시간 초기화
	TimeSetup(&startTime);
	// 플레이어 점수 초기화
	player.score = 0;
	// 플레이어 위치 초기화
	player.x = (SCREEN_WIDTH / 2) - 3;
	player.y = SCREEN_HEIGHT - 7;
	// 시간 표기 초기화
	scoreAndTimeUI[1][1] = '0';
	// 뮤직 바 초기화
	playbarPos = 6;
	scoreAndTimeUI[1][6] = '|';
	for (int i = 7; i <= 36; i++)
		scoreAndTimeUI[1][i] = '-';
}

/// 플레이어 관련 함수
void CharacterMove()
{
	player.tick++;

	int upCollisionCount = 0;

	/// 바닥에 땅('/')이 있는지 확인
	if (CollisionCheck(COLLCHECK_DOWN, player.x, player.y, player.sizeX, player.sizeY) != COLLCHECK_DOWN)
		player.isJump = TRUE;

	// 플레이어 점프 활성화
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		upCollisionCount = 0;

		//점프 안 뛰고 있을 때만 활성화
		if (player.isJump == FALSE)
		{
			player.jump = 0;
			upCollisionCount = CollisionCheck(COLLCHECK_UP, player.x, player.y, player.sizeX, player.sizeY);
			player.y -= upCollisionCount; // 점푸
			player.isJump = TRUE;
		}
	}

	// 플레이어 왼쪽 이동
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && player.x <= SCREEN_WIDTH - player.sizeX)
	{
		if (player.x > 0) 
		{
			// 방향 오른쪽이었으면 tick 초기화
			if (player.direct == DIRECT_RIGHT)
				player.tick = 0;
			// x축으로 -1 이동
			player.x -= 1;
			// 방향 왼쪽 설정
			player.direct = DIRECT_LEFT;
			// 애니메이션 설정
			if (player.tick <= 10) //0~10
				player.animation = LEFT_WALK_1;
			else if (player.tick <= 20)  //10~20
				player.animation = LEFT_WALK_2;
			else player.tick = 0;
		}
	}

	// 플레이어 오른쪽 이동
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && player.x >= 0)
	{
		if (player.x < SCREEN_WIDTH - player.sizeX) 
		{
			// 방향 왼쪽이었으면 tick 초기화
			if (player.direct == DIRECT_LEFT)
				player.tick = 0;
			// x축으로 +1 이동
			player.x += 1;
			// 방향 오른쪽 설정
			player.direct = DIRECT_RIGHT;
			// 애니메이션 설정
			if (player.tick <= 10)
				player.animation = RIGHT_WALK_1;
			else if (player.tick <= 20)
				player.animation = RIGHT_WALK_2;
			else player.tick = 0;
		}
	}

	if (player.isAttack == TRUE)
		player.attackTick++;

	// 플레이어 공격
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (player.isAttack == FALSE) {
			player.isAttack = TRUE;
		}
			

		if (player.direct == DIRECT_LEFT)
			player.animation = LEFT_ATTACK;
		else if (player.direct == DIRECT_RIGHT)
			player.animation = RIGHT_ATTACK;
	}

	// 공격 취한 후 tick 10 지나면 평상시 자세로 돌아옴
	if ((player.animation == LEFT_ATTACK || player.animation == RIGHT_ATTACK) && player.attackTick >= 7)
	{
		if (player.direct == DIRECT_LEFT) player.animation = LEFT_WALK_1;
		else if (player.direct == DIRECT_RIGHT) player.animation = RIGHT_WALK_1;

		player.isAttack = FALSE;
		player.attackTick = 0;
	}

	CharacterJump();
	CharacterDraw(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, player.animation);
}

void CharacterJump()
{

	if (player.isJump == TRUE)
	{
		// 공격중일 때
		if (player.isAttack == TRUE)
		{ 
			if (player.direct == DIRECT_LEFT)
				player.animation = LEFT_ATTACK;
			else if (player.direct == DIRECT_RIGHT)
				player.animation = RIGHT_ATTACK;
		} 
		// 공격중이지 않을때
		else 
		{ 
			//점프 애니메이션
			if (player.direct == DIRECT_LEFT)
				player.animation = LEFT_JUMP;
			else if (player.direct == DIRECT_RIGHT)
				player.animation = RIGHT_JUMP;
		}

		if (player.jump < 1)
			player.jump += 0.2;

		if (CollisionCheck(COLLCHECK_DOWN, player.x, player.y, player.sizeX, player.sizeY) != COLLCHECK_DOWN) 
			player.y += floor(player.jump);
		else 
		{ 
			// 제자리로
			player.isJump = FALSE;
			player.jump = 0;
			// 점프공격 초기화
			player.isAttack = FALSE;
			//점프 끝나고 돌아오는 애니메이션
			if (player.direct == DIRECT_LEFT)
				player.animation = LEFT_WALK_1;
			else if (player.direct == DIRECT_RIGHT)
				player.animation = RIGHT_WALK_1;
		}

	}
}

void CharacterDraw(int color, int anim) 
{
	for (int i = 0; i < player.sizeY; i++) 
	{
		for (int j = 0; j < player.sizeX; j++) 
		{
			backBuffer[player.y + i][player.x + j].Char.AsciiChar = player.sprite[anim][i][j];
			backBuffer[player.y + i][player.x + j].Attributes = color;
		}
	}
}

/// 당근 관련 함수
void CarrotManagement() 
{
	if (isGenerate == NONE) 
	{
		// pCarrot[i]는 0~6까지 총 7개
		for (int i = 0; i < CARROT_COUNT; i++) 
			pCarrot[i] = CarrotGenerator();

		isGenerate = EXE;
	}
	else if (isGenerate == EXE)
	{
		carrotDieCount = 0;
		for (int i = 0; i < CARROT_COUNT; i++)
		{
			CarrotCreate(pCarrot[i]);

			// 만약 당근이 모두 죽었을 경우
			if (pCarrot[i]->isGenerated == FALSE)
				carrotDieCount++;
			// 당근 재생성
			if (carrotDieCount == CARROT_COUNT) 
				isGenerate = FREE;
		}
	}
	else 
	{
		// 당근 동적할당 해제
		for (int j = 0; j < CARROT_COUNT; j++)
			free(pCarrot[j]);
		isGenerate = NONE;
	}
}

Carrot* CarrotGenerator()
{
	/// 당근 생성 좌표
	// 총 40개
	CarrotPos carrotPos[] = 
	// 바닥
	{ {5,FLOOR_0F}, {5,FLOOR_0F}, {12,FLOOR_0F}, {19,FLOOR_0F}, {26,FLOOR_0F}, {33,FLOOR_0F}, {40,FLOOR_0F},
	  {47,FLOOR_0F}, {54,FLOOR_0F}, {61,FLOOR_0F}, {68,FLOOR_0F}, {75,FLOOR_0F}, {82,FLOOR_0F}, 
	  {89,FLOOR_0F}, {96,FLOOR_0F}, {103,FLOOR_0F}, {110,FLOOR_0F}, {117,FLOOR_0F}, {124,FLOOR_0F}, 
	  {131,FLOOR_0F}, {138,FLOOR_0F}, 
	// 1층
	  {33,FLOOR_1F}, {140,FLOOR_1F},
	// 2층
	  {100,FLOOR_2F}, {107,FLOOR_2F}, {114,FLOOR_2F}, {121,FLOOR_2F},
	// 3층
	  {46,FLOOR_3F}, {53,FLOOR_3F}, {60,FLOOR_3F},
	// 4층
	  {6,FLOOR_4F}, {13,FLOOR_4F}, {20,FLOOR_4F}, {27,FLOOR_4F}, {34,FLOOR_4F},
	  {133,FLOOR_4F}, {140,FLOOR_4F},
    // 5층
	  {71,FLOOR_5F}, {78,FLOOR_5F}, {85,FLOOR_5F}, {92,FLOOR_5F} };

	int randomPos = 0;

	if (isCreated == TRUE)
	{
		for (int k = 0; k < CARROT_COUNT; k++)
			carrotPosCheck[k] = 0; // 중복 체크를 위한 배열 초기화
		index = 0;
		isCreated = FALSE;
	}

	/// 좌표 랜덤 생성 & 중복 방지
	// 당근 생성이 다 안되어 있으면
	if (isCreated == FALSE) 
	{ 
		randomPos = 1 + (rand() % 40); // 랜덤 난수 생성
		for (int i = 0; i < CARROT_COUNT; i++) 
		{
			// 중복된거 있으면
			if (carrotPosCheck[i] == randomPos) 
			{ 
				randomPos = 1 + (rand() % 40); // 다시 돌리기
				i = -1; // 다시 검사
			}
		}
		if(index != 7)
			carrotPosCheck[index++] = randomPos; // 중복된거 없으면 넣고, index 증가
		if (carrotPosCheck[CARROT_COUNT - 1] != 0) //마지막 값이 0이 아니면(좌표값이 들어가있으면)
			isCreated = TRUE;
	}

	// 생성, 홀드타임 랜덤 생성
	int randomHoldTime = 100 + (rand() % 200);
	int randomGenerateTick = 50 + (rand() % 50);
	Carrot* pCarrot = (Carrot*)malloc(sizeof(Carrot));
	CarrotInit(pCarrot, carrotPos[randomPos].x, carrotPos[randomPos].y, randomHoldTime, randomGenerateTick);
	return pCarrot;

}

// Carrot 구조체 초기화 함수
Carrot* CarrotInit(Carrot* pCarrot, short posX, short posY, int hTime, int genTime) 
{
	pCarrot->x = posX;
	pCarrot->y = posY;

	pCarrot->sizeX = 7;
	pCarrot->sizeY = 4;

	pCarrot->animation = 0;
	pCarrot->tick = 0;
	pCarrot->holdingTime = hTime;
	pCarrot->generateTick = genTime;

	pCarrot->isGenerated = TRUE;
	pCarrot->isDamaged = FALSE;
	pCarrot->dieAnimation = FALSE;
	pCarrot->disappearAnimation = FALSE;

	pCarrot->sprite = carrotSprite;

	return pCarrot;
}

void CarrotCreate(Carrot* pCarrot)
{
	if (pCarrot == NULL) return;

	pCarrot->tick++;

	// isGenerated = TRUE일때
	if (pCarrot->isGenerated == TRUE) 
	{

		// 당근 옆에 있는 플레이어가 isAttack = TRUE 상태일때 충돌처리
		if (CollisionCheck(COLLCHECK_X, pCarrot->x, pCarrot->y, pCarrot->sizeX, pCarrot->sizeY) == COLLCHECK_X
			&& player.isAttack == TRUE)
			pCarrot->isDamaged = TRUE;

		/// 당근이 사라지는 조건

		// 1. isDamaged = TRUE
		if (pCarrot->isDamaged == TRUE) 
		{
			pCarrot->dieAnimation = TRUE;
			pCarrot->tick = 0;
			pCarrot->isDamaged = FALSE;
		}

		// 2. 일정 시간 지났을 때
		//	  점수때문에 따로 뺐다
		if (pCarrot->tick >= pCarrot->generateTick + pCarrot->holdingTime) 
		{
			pCarrot->disappearAnimation = TRUE;
			pCarrot->tick = 0;
		}

		/// 당근 애니메이션

		// dieAnimation = FALSE일때 생성 애니메이션 실행
		if (pCarrot->tick > pCarrot->generateTick && pCarrot->dieAnimation == FALSE)
		{
			if (pCarrot->tick <= pCarrot->generateTick + 3) pCarrot->animation = CARROT_UP_1;
			else if (pCarrot->tick <= pCarrot->generateTick + 5) pCarrot->animation = CARROT_UP_2;
			else if (pCarrot->tick <= pCarrot->generateTick + 8) pCarrot->animation = CARROT_UP_3;
			else if (pCarrot->tick <= pCarrot->generateTick + 11) pCarrot->animation = CARROT_UP_4;
			else pCarrot->animation = CARROT_UP_5;
		}

		// dieAnimation = TRUE일때 쥬금 애니메이션 실행
		if (pCarrot->dieAnimation == TRUE)
		{
			if (pCarrot->tick <= 3) pCarrot->animation = CARROT_DOWN_1;
			else if (pCarrot->tick <= 5) pCarrot->animation = CARROT_DOWN_2;
			else if (pCarrot->tick <= 8) pCarrot->animation = CARROT_DOWN_3;
			else if (pCarrot->tick <= 11) pCarrot->animation = CARROT_DOWN_4;
			else 
			{
				pCarrot->animation = CARROT_DOWN_5;
				pCarrot->isGenerated = FALSE;
				// 당근 죽으면 플레이어 점수 추가
				player.score++;
			}
		}

		// disappearAnimation = TRUE일때 사라짐 애니메이션 실행
		if (pCarrot->disappearAnimation == TRUE)
		{
			if (pCarrot->tick <= 3) pCarrot->animation = CARROT_UP_5;
			else if (pCarrot->tick <= 5) pCarrot->animation = CARROT_UP_4;
			else if (pCarrot->tick <= 8) pCarrot->animation = CARROT_UP_3;
			else if (pCarrot->tick <= 11) pCarrot->animation = CARROT_UP_2;
			else {
				pCarrot->animation = CARROT_UP_1;
				pCarrot->isGenerated = FALSE;
			}
		}

		CarrotDraw(pCarrot, 6 | FOREGROUND_INTENSITY, pCarrot->animation);
	} 
}

void CarrotDraw(Carrot* pCarrot, int color, int anim)
{
	for (int i = 0; i < pCarrot->sizeY; i++)
	{
		for (int j = 0; j < pCarrot->sizeX; j++)
		{

			// 당근 스프라이트 공백처리
			if (pCarrot->sprite[anim][i][j] == ' ') continue;
			if (pCarrot->sprite[anim][i][j] == '%') backBuffer[pCarrot->y + i][pCarrot->x + j].Char.AsciiChar = ' ';
			else backBuffer[pCarrot->y + i][pCarrot->x + j].Char.AsciiChar = pCarrot->sprite[anim][i][j];

			backBuffer[pCarrot->y + i][pCarrot->x + j].Attributes = color;
		}
	}
}

/// 충돌 관련 함수
int CollisionCheck(int direct, int posX, int posY, int sizeX, int sizeY)
{

	/// 캐릭터 점프와 관련된 충돌체크

	/// 아래 충돌체크
	if (direct == COLLCHECK_DOWN)
	{
		for(int x = posX; x < posX + sizeX; x++)
			if (backBuffer[posY + sizeY][x].Char.AsciiChar == '/')
				return COLLCHECK_DOWN;
	}

	/// 위 충돌체크
	else if (direct == COLLCHECK_UP)
	{
		for (int y = 0; y <= player.jumpPower; y++)
		{
			for (int x = posX; x < posX + sizeX; x++)
			{
				if (backBuffer[posY - y][x].Char.AsciiChar == '/' || backBuffer[posY - y][x].Char.AsciiChar == NULL) 
					// 플레이어 좌표에서 몇칸 위에 발판 있는지 반환
					return y - 1;
			}
		}
		//발판 없으면 기본 점프 수치 반환		
		return player.jumpPower;
	}
	
	/// 캐릭터가 당근을 때렸는지 검사하기 위한 충돌체크

	/// 충돌체크
	else if (direct == COLLCHECK_X)
	{
		for (int y = posY; y < posY + sizeY; y++)
		{
			for(int x = posX - 1; x < posX + sizeX + 1; x++)
			// '@'문자가 있으면 충돌감지
			if (backBuffer[y][x].Char.AsciiChar == '@')
				return COLLCHECK_X;
		}
	}

	else return COLLCHECK_NO;
}

/// 화면을 출력하는 함수들
/// Double Buffering
void MapDraw()
{
	FILE* fp;
	errno_t err;

	// 맵 파일 열기
	err = fopen_s(&fp, "map\\map.txt", "r");

	// 맵 파일 열기 실패하면 return
	if (err != 0)
	{
		printf(" file open failed");
		return;
	}

	// 맵 정보 받는 이차원배열 동적할당
	map = (char**)malloc(sizeof(char*) * SCREEN_HEIGHT);
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		map[i] = (char*)malloc(sizeof(char) * SCREEN_WIDTH);

	// 맵 정보 이차원배열에 복사
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		for (int j = 0; j < SCREEN_WIDTH; j++)
			fscanf_s(fp, "%c", &map[i][j], 1);

	// 맵 파일 닫기
	fclose(fp);

	// 이중버퍼에 맵 그리기
	for (int i = 0; i < SCREEN_HEIGHT-1; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH-1; j++)
		{
			if (map[i][j] == '0') backBuffer[i][j].Char.AsciiChar = ' ';
			else if (map[i][j] == '*')
			{
				backBuffer[i][j].Char.AsciiChar = map[i][j];
				backBuffer[i][j].Attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			}
			else
			{
				backBuffer[i][j].Char.AsciiChar = map[i][j];
				backBuffer[i][j].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; //게임 시작 화면 맵 색
			}
		}
	}
}

void MapDie()
{
	//맵 동적할당 해제
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		free(map[i]);
	free(map);
}

// 이중 버퍼 초기화
void BufferReset()
{
	for (int i = 0; i < SCREEN_HEIGHT - 1; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH - 1; j++)
		{
			if (map[i][j] == '0') backBuffer[i][j].Char.AsciiChar = ' ';
			else if (map[i][j] == '*')
			{
				backBuffer[i][j].Char.AsciiChar = map[i][j];
				backBuffer[i][j].Attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			}
			else
			{
				backBuffer[i][j].Char.AsciiChar = map[i][j];
				backBuffer[i][j].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; //게임 화면 맵 색
			}
		}
	}
}

// backBuffer를 screenBuffer에 복사
void Render()
{
	memcpy(pScreenBuffer, backBuffer, sizeof(backBuffer));
}

// screenBuffer를 콘솔에 출력
void Present()
{
	COORD bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
	COORD bufferCoord = { 0, 0 };
	SMALL_RECT writeRegion = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleOutput(hStdOut, (CHAR_INFO*)screenBuffer, bufferSize, bufferCoord, &writeRegion);
}

//-----------------------------------------------------------

/// INGAME UI

void ScoreAndTimeUI()
{
	/// 진행시간 표시 UI
	currentTime = GetTickCount64();
	// 경과 시간(초)
	int temElapsedTime = elapsedTime;
	elapsedTime = floor(difftime(currentTime, gameLoopStartTime)) / 1000;
	secondBack = elapsedTime % 10;
	secondFront = (elapsedTime / 10) % 6;
	scoreAndTimeUI[1][4] = (char)(secondBack + 48);
	scoreAndTimeUI[1][3] = (char)(secondFront + 48);
	
	// 뮤직 바 위치가 36보다 작고
	// 2초에 한번!!! 실행해야 함
	if (elapsedTime != 0 && playbarPos < 36 && elapsedTime != temElapsedTime && elapsedTime % 2 == 0)
	{
		//2초에 한번씩 뮤직 바 옮기기
		scoreAndTimeUI[1][playbarPos] = '=';
		playbarPos++;
		scoreAndTimeUI[1][playbarPos] = '|';
		temElapsedTime = elapsedTime;
	}

	/// 게임 종료시
	if (elapsedTime >= GAME_TIME_LIMIT)
	{
		// 인게임 시간표시 UI 고정
		scoreAndTimeUI[1][1] = '1';
		scoreAndTimeUI[1][4] = '0';
		scoreAndTimeUI[1][3] = '0';

		/// ★★★★★★★★★
		/// 게임루프 모드 변경
		gameMode = ENDGAME;
	}

	/// 점수 UI
	scoreThird = player.score % 10;
	scoreSecond = (player.score / 10) % 10;
	scoreFirst = (player.score / 100) % 10;
	scoreAndTimeUI[0][26] = (char)(scoreThird + 48);
	scoreAndTimeUI[0][25] = (char)(scoreSecond + 48);
	scoreAndTimeUI[0][24] = (char)(scoreFirst + 48);

	UIDraw();
}

void UIDraw()
{
	//back buffer에 그리기
	for (int i = UI_POS_Y; i < UI_POS_Y + UI_SIZE_Y; i++)
	{
		for (int j = UI_POS_X; j < UI_POS_X + UI_SIZE_X; j++)
		{
			backBuffer[i][j].Char.AsciiChar = scoreAndTimeUI[i - UI_POS_Y][j - UI_POS_X];
			if (scoreAndTimeUI[i - UI_POS_Y][j - UI_POS_X] == '=' || scoreAndTimeUI[i - UI_POS_Y][j - UI_POS_X] == '|') backBuffer[i][j].Attributes = FOREGROUND_BLUE;
			else if (scoreAndTimeUI[i - UI_POS_Y][j - UI_POS_X] == '*') backBuffer[i][j].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			else backBuffer[i][j].Attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		}
	}
}

//-----------------------------------------------------------

/// OUTGAME UI

void TimeOverDraw()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 기본 맵 위에 출력할거얌
	MapDraw();
	Render();
	Present();

	GotoXY(24,  4); printf(".----------------------------------------------------------------------------------------------------.");
	GotoXY(24,  5); printf("| |♥|catching carrot.exe                                                                         |x| |");
	GotoXY(24,  6); printf("|----------------------------------------------------------------------------------------------------|");
	GotoXY(24,  7); printf("|                                                                                                    |");
	GotoXY(24,  8); printf("|                                                                               _\\|/_                |");
	GotoXY(24,  9); printf("|       /)_/)            (\\(\\         /)/)                         (\\(\\        /______\\              |");
	GotoXY(24, 10); printf("|     (  '-')           _( ._.)_    ( '-')                        _(._. )_     \\ '-'  /              |");
	GotoXY(24, 11); printf("|     /     '._ ,-.-/) (,--.   ,--.)(,------.       /)/),-----. ,--(_/   ,.\\ ,------.,------.        |");
	GotoXY(24, 12); printf("|     |'--...__)|  |'')|   `.'   |  |  .---'      ('-' / .-.  '\\   \\   /(__/ |  .---'|   /`. '       |");
	GotoXY(24, 13); printf("|     '--.  .--'|  |  \\|         |  |  |          /   |  | |  | \\   \\ /   /  |  |    |  /  | |       |");
	GotoXY(24, 14); printf("|        |  |   |  |(_/|  |'.'|  |  |  '--.       \\_) |  |\\|  |  \\   '   /, (|  '--. |  |_.' |       |");
	GotoXY(24, 15); printf("|        |  |  ,|  |_.'|  |   |  |  |  .--'         \\ |  | |  |   \\     /__) |  .--' |  .  '.'       |");
	GotoXY(24, 16); printf("|        |  | (_|  |   |  |   |  |  |  `---.         `'  '-'  '    \\   /     |  `---.|  |\\  \\        |");
	GotoXY(24, 17); printf("|        `--'   `--'   `--'   `--'  `------'           `-----'      `-'      `------'`--' '--'       |");
	GotoXY(24, 18); printf("|                                                                                                    |");
	GotoXY(24, 19); printf("|                                                                                                    |");
	GotoXY(24, 20); printf("|                                                                                                    |");
	GotoXY(24, 21); printf("|                                                                                                    |");
	GotoXY(24, 22); printf("|                                                                                                    |");
	GotoXY(24, 23); printf("|____________________________________________________________________________________________________|");

	ShowScore(hConsole);
	RankingRegister(hConsole);

	Sleep(3000);
	ScreenCloseOpen();
	Sleep(1000);
}

void ShowScore(HANDLE hConsole)
{
	/// 점수 출력

	GotoXY(55, 20);
	SET_GREEN printf("|♥| SCORE : %03d / ", player.score);
	SET_WHITE printf("PRESS "); SET_YELLOW printf("SPACE"); SET_WHITE printf(" TO CONTINUE");
	Sleep(1000);

	while (1)
	{
		if (menuControl() == SPACE)
		{
			GotoXY(55, 20);
			printf("|♥|                                       ");
			break;
		}
	}

}

void RankingRegister(HANDLE hConsole)
{
	/// 이름 입력받기
	// 배열 초기화
	memset(name, '\0', sizeof(name));

	GotoXY(55, 20);
	SET_GREEN printf("|♥|");
	SET_WHITE printf(" 이름 입력해죠(5글자 이내루) : ");

	// 특수키 제거
	while (_kbhit())
	{
		char c = _getch();
	}

	// 이름 입력받기
	fgets(name, sizeof(name), stdin);

	// name 크기보다 글자 많이 들어갔을 때 입력버퍼 제거
	if (strlen(name) == sizeof(name) - 1 && name[sizeof(name) - 2] != '\n')
	{
		int c = 0;
		while ((c = getchar()) != '\n' && c != EOF);
	}

	// 개행문자 제거
	name[strcspn(name, "\n")] = '\0';

	GotoXY(62, 21);
	SET_YELLOW printf("ㄴ Thanks for playing %s ♥",name);
	SET_WHITE

	// 공백 제거
	RemoveSpaces(name);

	/// .txt파일에 쓰기
	FILE* pf;
	errno_t err;

	err = fopen_s(&pf, "rank\\ranking.txt", "at");
	if (err != 0)
	{
		GotoXY(0, 0);
		printf("Error opening file\n");
	}
	else
	{
		fprintf(pf, "%s %d\n", name, player.score);
		fclose(pf);
	}

}

void RemoveSpaces(char *str)
{
	int i = 0, j = 0;
	while (str[i])
	{
		if (str[i] != ' ') //str[0]이 공백이 아니면
			str[j++] = str[i]; //str[0] 에 str[0] 값 넣어주고 j++ -> str[1] 됨
		i++;
	}

	str[j] = '\0';
}

void ScreenCloseOpen()
{
	for (int i = 0; i < SCREEN_HEIGHT - 1; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH - 1; j++)
		{
			backBuffer[i][j].Char.AsciiChar = ' ';
			backBuffer[i][j].Attributes = NULL;
			Render();
			Present();
		}
	}
	Sleep(500);

	for (int i = 0; i < SCREEN_HEIGHT - 1; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH - 1; j++)
		{
			if (map[i][j] == '0') backBuffer[i][j].Char.AsciiChar = ' ';
			else if (map[i][j] == '*')
			{
				backBuffer[i][j].Char.AsciiChar = map[i][j];
				backBuffer[i][j].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}
			else
			{
				backBuffer[i][j].Char.AsciiChar = map[i][j];
				backBuffer[i][j].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}
			Render();
			Present();
		}
	}

}


void TitleDraw()
{
	system("cls");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 기본 맵 위에 출력할거얌
	MapDraw();
	Render();
	Present();

	// 좌우 이동 튜토리얼
	SET_GREEN GotoXY( 9, 18); printf("◀----------"); SET_WHITE GotoXY(9, 19); printf(" '좌'로 이동");
	SET_GREEN GotoXY(11, 20); printf("----------▶"); SET_WHITE GotoXY(11, 21); printf(" '우'로 이동");

	// 점프 튜토리얼
	SET_GREEN GotoXY(12, 5); printf("▲");		SET_WHITE GotoXY(7, 8); printf(" '위'로");
	SET_GREEN GotoXY(12, 6); printf("|");		SET_WHITE GotoXY(10, 9); printf(" 점푸");
	SET_GREEN GotoXY(12, 7); printf("|");

	// 공격 튜토리얼
	SET_GREEN GotoXY(136, 15); printf("「 SHIFT 」");
	SET_WHITE GotoXY(130, 13); printf("공");
	SET_WHITE GotoXY(131, 14); printf("격!");

	// 선택 튜토리얼
	SET_WHITE GotoXY(134, 7); printf("메뉴");
	SET_WHITE GotoXY(135, 8); printf("선택은");
	SET_GREEN GotoXY(137, 9); printf("★SPACE★");

	SET_WHITE GotoXY(130,  9); printf(" (\\_(\\ ");
	SET_WHITE GotoXY(130, 10); printf(" ( >.<)o");

	SET_WHITE
	GotoXY(5, 22); printf("  \\|/  ");			GotoXY(14, 6); printf("`(\\_(\\ ");
	GotoXY(5, 23); printf("/_____\\");			GotoXY(14, 7); printf(" o 'o')`");
	GotoXY(5, 24); printf("\\ '-' / ");			GotoXY(14, 8); printf(" `\\_)_)");
	GotoXY(5, 25); printf(" \\=  / ");
	
	GotoXY(132, 16); printf("  \\|/  ");		
	GotoXY(132, 17); printf("/_____\\");		GotoXY(139, 17); printf(",`/)_/) ");
	GotoXY(132, 18); printf("\\ x_x / ");		GotoXY(139, 18); printf("@('^' ) ");
	GotoXY(132, 19); printf(" \\=  / ");		GotoXY(139, 19); printf("  (_(_ o");

	GotoXY(24,  2); printf(".----------------------------------------------------------------------------------------------------.");
	GotoXY(24,  3); printf("| Whale_game.exe                                                                                      |x| |");
	GotoXY(24,  4); printf("|----------------------------------------------------------------------------------------------------|");
	GotoXY(24,  5); printf("|                                                                                                    |");
	GotoXY(24,  6); printf("|                                                                                                    |");
	GotoXY(24,  7); printf("|                                                                                                    |");
	GotoXY(24,  8); printf("|                                                                                                    |");
	GotoXY(24,  9); printf("|                                                                                                    |");
	GotoXY(24, 10); printf("|                                                                                                    |");
	GotoXY(24, 11); printf("|                                                                                                    |");
	GotoXY(24, 12); printf("|                                                                                                    |");
	GotoXY(24, 13); printf("|                                                                                                    |");
	GotoXY(24, 14); printf("|                                                                                                    |");
	GotoXY(24, 15); printf("|                                                                                                    |");
	GotoXY(24, 16); printf("|                                                                                                    |");
	GotoXY(24, 17); printf("|                                                                                                    |");
	GotoXY(24, 18); printf("|                                                                                                    |");
	GotoXY(24, 19); printf("|                                                                                                    |");
	GotoXY(24, 20); printf("|                                                                                                    |");
	GotoXY(24, 21); printf("|                                                                                                    |");
	GotoXY(24, 22); printf("|                                                                                                    |");
	GotoXY(24, 23); printf("|                                                                                                    |");
	GotoXY(24, 24); printf("|                                                                                                    |");
	GotoXY(24, 25); printf("|                                                                                                    |");
	GotoXY(24, 26); printf("|____________________________________________________________________________________________________|");

	SET_WHITE
	GotoXY(34,  6); printf("           hh              lll                                              ");
	GotoXY(34,  7); printf("ww      ww hh        aa aa lll   eee      gggggg   aa aa mm mm mmmm    eee  ");
	GotoXY(34,  8); printf("ww      ww hhhhhh   aa aaa lll ee   e    gg   gg  aa aaa mmm  mm  mm ee   e ");
	GotoXY(34,  9); printf(" ww ww ww  hh   hh aa  aaa lll eeeee     ggggggg aa  aaa mmm  mm  mm eeeee  ");
	GotoXY(34, 10); printf("  ww  ww   hh   hh  aaa aa lll  eeeee         gg  aaa aa mmm  mm  mm  eeeee ");
	GotoXY(34, 11); printf("                                          ggggg                             ");

	SET_WHITE
	GotoXY(44, 13); printf("O  ---(.______)--||-------||----||------||-------||-----");
	GotoXY(44, 14); printf("|                                                      |");
	GotoXY(44, 15); printf("|       이번 게임은 [내가 고래 였다면] 입니다.         |");
	GotoXY(44, 16); printf("|                                                      |");
	GotoXY(44, 17); printf("|      제한시간 내에 더 많은 게를 잡아 승리하세요.     |");
	GotoXY(44, 18); printf("|                                                      |");
	GotoXY(44, 19); printf("O  -----||-------||----||------||-----||----(______.)---");
}

void RankingDraw()
{
	system("cls");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 기본 맵 위에 출력할거얌
	MapDraw();
	Render();
	Present();
	
	GotoXY(24,  2); printf(".----------------------------------------------------------------------------------------------------.");
	GotoXY(24,  3); printf("|  Whale_game ranking.exe                                                                        |x| |");
	GotoXY(24,  4); printf("|----------------------------------------------------------------------------------------------------|");
	GotoXY(24,  5); printf("|                                                                                                    |");
	GotoXY(24,  6); printf("|                                                                                                    |");
	GotoXY(24,  7); printf("|                                                                                                    |");
	GotoXY(24,  8); printf("|                                                                                                    |");
	GotoXY(24,  9); printf("|                                                                                                    |");
	GotoXY(24, 10); printf("|                                                                                                    |");
	GotoXY(24, 11); printf("|                                                                                                    |");
	GotoXY(24, 12); printf("|                                                                                                    |");
	GotoXY(24, 13); printf("|                                                                                                    |");
	GotoXY(24, 14); printf("|                                                                                                    |");
	GotoXY(24, 15); printf("|                                                                                                    |");
	GotoXY(24, 16); printf("|                                                                                                    |");
	GotoXY(24, 17); printf("|                                                                                                    |");
	GotoXY(24, 18); printf("|                                                                                                    |");
	GotoXY(24, 19); printf("|                                                                                                    |");
	GotoXY(24, 20); printf("|                                                                                                    |");
	GotoXY(24, 21); printf("|                                                                                                    |");
	GotoXY(24, 22); printf("|                                                                                                    |");
	GotoXY(24, 23); printf("|                                                                                                    |");
	GotoXY(24, 24); printf("|                                                                                                    |");
	GotoXY(24, 25); printf("|                                                                                                    |");
	GotoXY(24, 26); printf("|____________________________________________________________________________________________________|");

	/// 랭킹 담을 구조체
	Rank rank[200] = { "\0", 0 };
	RankingRoad(rank);

	GotoXY(53, 11); printf("               ,---------------.               ");
	GotoXY(53, 12); printf("               |               |               ");
	GotoXY(53, 13); printf("               |               |               ");
	GotoXY(53, 14); printf(",--------------'               |               ");
	GotoXY(53, 15); printf("|                              `--------------.");
	GotoXY(53, 16); printf("|                                             |");
	GotoXY(53, 17); printf("|                                             |");
	GotoXY(53, 18); printf("|                                             |");
	GotoXY(53, 19); printf("'---------------------------------------------'");

	// 뒤로가기 튜토리얼
	SET_WHITE GotoXY(132, 7); printf("메인메뉴로");
	SET_WHITE GotoXY(136, 8); printf("돌아가기");
	SET_GREEN GotoXY(138, 9); printf("★SPACE★");

	SET_WHITE GotoXY(130, 9); printf(" (\\_(\\ ");
	SET_WHITE GotoXY(130, 10); printf(" ( >.<)o");

	//1등
	if (rank[0].score != 0)
	{
		SET_YELLOW
		GotoXY(71, 6); printf(" (\\_(\\ ");
		GotoXY(71, 7); printf(" o >.<) ");
		GotoXY(71, 8); printf("  \\_)_) ");
		GotoXY(71, 10); printf("    \\\\ ");
			
		SET_GREEN GotoXY(71 + (5 - strlen(rank[0].name) / 2), 12); printf("%s", rank[0].name);
		SET_YELLOW GotoXY(74, 13); printf("%d점", rank[0].score);
	}

	//2등
	if (rank[1].score != 0)
	{
		SET_YELLOW
		GotoXY(57, 11); printf(" /)_/) ");
		GotoXY(57, 12); printf("( ^-^)v");
		GotoXY(57, 13); printf("o _)_) ");

		SET_GREEN GotoXY(55 + (5 - strlen(rank[1].name) / 2), 15); printf("%s", rank[1].name);
		SET_YELLOW GotoXY(59, 16); printf("%d점", rank[1].score);
	}

	//3등
	if (rank[2].score != 0)
	{
		SET_YELLOW
		GotoXY(89, 12); printf("  /)_/) ");
		GotoXY(89, 13); printf("v(o.< ) ");
		GotoXY(89, 14); printf("  (_(_ o");

		SET_GREEN GotoXY(88 + (5 - strlen(rank[2].name) / 2), 16); printf("%s", rank[2].name);
		SET_YELLOW GotoXY(91, 17); printf("%d점", rank[2].score);
	}

	for (int i = 3; i < 7; i++)
	{
		if (rank[i].score == 0) break;
		GotoXY(68, 21 + (i-3) );
		SET_YELLOW printf("[%d등]", i+1);
		SET_GREEN printf(" %s", rank[i].name);
		SET_YELLOW printf(" : %d점", rank[i].score);
	}

	SET_WHITE
	while (1)
		if (menuControl() == SPACE)
		{
			break;
		}
}

void RankingRoad(Rank* rank)
{

	/// .txt파일 읽어오기
	FILE* pf;
	errno_t err;
	int index = 0;

	err = fopen_s(&pf, "rank\\ranking.txt", "r");
	if (err != 0)
	{
		GotoXY(0, 0);
		printf("Error opening file\n");
	}
	else
	{
		while (EOF != fscanf_s(pf,"%s %d", rank[index].name, MAX_NAME_LEN+1, &(rank[index].score)))
		{
			index++;
		}
		fclose(pf);
	}

	BubbleSort(rank, index);
}

void BubbleSort(Rank* arr, int size)
{
	int i, j;
	Rank temp;

	for (i = 0; i < size - 1; i++)
	{
		for (j = 0; j < size - 1 - i; j++)
		{
			if (arr[j].score < arr[j + 1].score)
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

int MenuDraw()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int mode = 0;
	int n = 0;
	StartMenuDraw(hConsole);

	// 키보드 입력을 기다리는 부분
	// 스페이스바가 입력되면 return하여 빠져나감
	while (1)
	{
		n = 0;
		//키보드 이벤트를 키값으로 받아오기
		n = menuControl();

		switch (n)
		{

			case RIGHT:
			{

				if (mode == 0)
				{
					RankingMenuDraw(hConsole);
					mode = 1;
				}

				else if (mode == 1)
				{
					ExitMenuDraw(hConsole);
					mode = 2;
				}

				break;
			}

			case LEFT:
			{

				if (mode == 2)
				{
					RankingMenuDraw(hConsole);
					mode = 1;
				}

				else if (mode == 1)
				{
					StartMenuDraw(hConsole);
					mode = 0;
				}

				break;
			}

			case SPACE:
			{
				// 스페이스바(선택)되었을 경우
				return mode;
			}
		}
	}
}

void StartMenuDraw(HANDLE hConsole)
{
	GotoXY(55, 25);
	SET_GREEN printf("|♥| START");
	GotoXY(69, 25);
	SET_YELLOW printf("    RANKING");
	GotoXY(86, 25);
	SET_YELLOW printf("    EXIT");

	SET_WHITE
}

void RankingMenuDraw(HANDLE hConsole)
{
	GotoXY(55, 25);
	SET_YELLOW printf("    START");
	GotoXY(69, 25);
	SET_GREEN printf("|♥| RANKING");
	GotoXY(86, 25);
	SET_YELLOW printf("    EXIT");

	SET_WHITE
}

void ExitMenuDraw(HANDLE hConsole)
{
	GotoXY(55, 25);
	SET_YELLOW printf("    START");
	GotoXY(69, 25);
	SET_YELLOW printf("    RANKING");
	GotoXY(86, 25);
	SET_GREEN printf("|♥| EXIT");

	SET_WHITE
}