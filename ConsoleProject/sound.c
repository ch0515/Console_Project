#include "main.h"
#include "sound.h"

FMOD_SYSTEM* g_FMODSystem;
//하나만 저장할 수 있음 //우리가 메모리에 올릴 수 있는 최소 사운드 단위 // 일단 세개만 ..
FMOD_SOUND* pSound[SOUND_LIMIT]; // PLAYER ATTACK, JUMP, 
FMOD_SOUND* pBGM; // BGM 1개
FMOD_CHANNEL *pChannel1, *pChannel2;

void CreateSoundInit()
{	
	// FMOD_System 어떻게든 만든다(?)
	FMOD_System_Create(&g_FMODSystem, FMOD_VERSION);
	// channel // 안겹치는 소리 개수
	FMOD_System_Init(g_FMODSystem, 32, FMOD_INIT_NORMAL, NULL);
}

// FMOD_SOUND -> 사운드를 나누는 식별자?
// 내가 설정하는 사운드를 전달하는 방법
void AllocateSound()
{
	// 파일명은 무조건 확장자 써줘야함
	// 테스트로 점프 사운드 생성
	// MODE : FMOD_DEFAULT 한번만 재생할거당 // FMOD_LOOP_NORMAL 끝나고도 반복할거당
	FMOD_System_CreateSound(g_FMODSystem, "bgm\\bgm.mp3", FMOD_LOOP_NORMAL, 0, &pSound[SOUND_BGM]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\jump.wav", FMOD_DEFAULT, 0, &pSound[SOUND_JUMP]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\hit.wav", FMOD_DEFAULT, 0, &pSound[SOUND_HIT]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\select.wav", FMOD_DEFAULT, 0, &pSound[SOUND_SELECT]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\click.wav", FMOD_DEFAULT, 0, &pSound[SOUND_CLICK]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\gen.wav", FMOD_DEFAULT, 0, &pSound[SOUND_GEN]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\carrotdie.wav", FMOD_DEFAULT, 0, &pSound[SOUND_CARROTDIE]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\screenopen.wav", FMOD_DEFAULT, 0, &pSound[SOUND_SCREENOPEN]);
	FMOD_System_CreateSound(g_FMODSystem, "effect\\screenclose.wav", FMOD_DEFAULT, 0, &pSound[SOUND_SCREENCLOSE]);

}

void PlayBGM()
{
	FMOD_System_PlaySound(g_FMODSystem, pSound[SOUND_BGM], NULL, 0, &pChannel2);
	FMOD_Channel_SetVolume(pChannel2, 0.4f);
}

void PlaySoundEffect(int effect)
{
	FMOD_System_PlaySound(g_FMODSystem, pSound[effect], NULL, 0, &pChannel1);
	//0~1까지의 볼륨 //내가 윈도우에서 설정한 볼륨의 0.x만큼 
	//PlaySound에 사운드를 할당 하고 볼륨 조절해야 작동함 //안하면 오류
	FMOD_Channel_SetVolume(pChannel1, 0.6f);
}

void ReleaseSound()
{
	FMOD_Sound_Release(pBGM);
	for (int i = 0; i < SOUND_LIMIT; i++)
		FMOD_Sound_Release(pSound[i]);	

	FMOD_System_Close(g_FMODSystem);
	FMOD_System_Release(g_FMODSystem);
}

// 음악 다른거 재생할때 끊고 다시 실행시켜줘야함!
void StopSound()
{
	FMOD_Channel_Stop(&pChannel1);
}
