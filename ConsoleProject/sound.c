#include "main.h"
#include "sound.h"

FMOD_SYSTEM* g_FMODSystem;
//�ϳ��� ������ �� ���� //�츮�� �޸𸮿� �ø� �� �ִ� �ּ� ���� ���� // �ϴ� ������ ..
FMOD_SOUND* pSound[SOUND_LIMIT]; // PLAYER ATTACK, JUMP, 
FMOD_SOUND* pBGM; // BGM 1��
FMOD_CHANNEL *pChannel1, *pChannel2;

void CreateSoundInit()
{	
	// FMOD_System ��Ե� �����(?)
	FMOD_System_Create(&g_FMODSystem, FMOD_VERSION);
	// channel // �Ȱ�ġ�� �Ҹ� ����
	FMOD_System_Init(g_FMODSystem, 32, FMOD_INIT_NORMAL, NULL);
}

// FMOD_SOUND -> ���带 ������ �ĺ���?
// ���� �����ϴ� ���带 �����ϴ� ���
void AllocateSound()
{
	// ���ϸ��� ������ Ȯ���� �������
	// �׽�Ʈ�� ���� ���� ����
	// MODE : FMOD_DEFAULT �ѹ��� ����ҰŴ� // FMOD_LOOP_NORMAL ������ �ݺ��ҰŴ�
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
	//0~1������ ���� //���� �����쿡�� ������ ������ 0.x��ŭ 
	//PlaySound�� ���带 �Ҵ� �ϰ� ���� �����ؾ� �۵��� //���ϸ� ����
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

// ���� �ٸ��� ����Ҷ� ���� �ٽ� ������������!
void StopSound()
{
	FMOD_Channel_Stop(&pChannel1);
}
