/// Catching Carrot
/// Entry Point
/// 2023.04.17 - 04.28
/// @nyong_u_u

#include "main.h"
#include "game.h"

extern ULONGLONG startTime;
int gameMode = STARTGAME;

int main() 
{

	/// Init
	Init();

	while (TRUE)
	{

		/// Title & Menu Select
		if (gameMode == STARTGAME)
		{
			StartGame();
		}
	
		//------------------------------------------

		/// Loop
		if (gameMode == GAMELOOP)
		{
			if (GetTickCount64() - startTime > 30)
			{
				UpdateGame();
				startTime = GetTickCount64();
			}
		}
	
		//------------------------------------------

		if (gameMode == ENDGAME)
		{

		/// Ranking
		/// Free
		EndGame();

		}

		//------------------------------------------

		/// EXIT
		if (gameMode == GAMEEXIT)
		{
			// 사운드 해제
			break;
		}
			
	}

	return 0;
}


