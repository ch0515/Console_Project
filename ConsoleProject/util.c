#include "main.h"

/// 콘솔 세팅
void SetConsole() 
{
	// 콘솔 사이즈 조절 및 타이틀 설정
	system("mode con: cols=150 lines=30 | title Catching Moles");

	// 커서 사이즈 조절 및 숨기기
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConsoleCursor;

	ConsoleCursor.bVisible = 0;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(consoleHandle, &ConsoleCursor);

	//마우스 입력 무시
	HANDLE consoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(consoleInputHandle, ENABLE_EXTENDED_FLAGS);

	// 콘솔 창 크기 변경 방지 (마우스로 조절 불가능)
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_SIZEBOX);

	// 전체화면 기능 방지
	LONG_PTR style = GetWindowLongPtr(consoleWindow, GWL_STYLE);
	style &= ~WS_MAXIMIZEBOX;
	SetWindowLongPtr(consoleWindow, GWL_STYLE, style);
}

/// 시간 세팅
void TimeSetup(ULONGLONG* time)
{
	// 시간 저장
	*time = GetTickCount64();
}

/// 커서 위치 이동 함수
void GotoXY(int x, int y)
{
	// 콘솔 핸들 가져오기
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(consoleHandle, pos);
}