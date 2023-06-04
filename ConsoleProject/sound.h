#pragma once



//-------------------------

#define SOUND_LIMIT 15

typedef enum {

	SOUND_BGM,
	SOUND_JUMP,
	SOUND_HIT,
	SOUND_SELECT,
	SOUND_CLICK,
	SOUND_GEN,
	SOUND_CARROTDIE,
	SOUND_SCREENOPEN,
	SOUND_SCREENCLOSE

} EffectSound;

void CreateSoundInit();
void AllocateSound();
void PlayBGM();
void PlaySoundEffect(int);
