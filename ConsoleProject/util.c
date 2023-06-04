#include "main.h"

/// �ܼ� ����
void SetConsole() 
{
	// �ܼ� ������ ���� �� Ÿ��Ʋ ����
	system("mode con: cols=150 lines=30 | title Catching Moles");

	// Ŀ�� ������ ���� �� �����
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConsoleCursor;

	ConsoleCursor.bVisible = 0;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(consoleHandle, &ConsoleCursor);

	//���콺 �Է� ����
	HANDLE consoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(consoleInputHandle, ENABLE_EXTENDED_FLAGS);

	// �ܼ� â ũ�� ���� ���� (���콺�� ���� �Ұ���)
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_SIZEBOX);

	// ��üȭ�� ��� ����
	LONG_PTR style = GetWindowLongPtr(consoleWindow, GWL_STYLE);
	style &= ~WS_MAXIMIZEBOX;
	SetWindowLongPtr(consoleWindow, GWL_STYLE, style);
}

/// �ð� ����
void TimeSetup(ULONGLONG* time)
{
	// �ð� ����
	*time = GetTickCount64();
}

/// Ŀ�� ��ġ �̵� �Լ�
void GotoXY(int x, int y)
{
	// �ܼ� �ڵ� ��������
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(consoleHandle, pos);
}