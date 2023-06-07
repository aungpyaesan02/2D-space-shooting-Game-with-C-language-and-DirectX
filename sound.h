//=============================================================================
//
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_sample000,		// BGM0
	SOUND_LABEL_BGM_sample001,		// BGM1
	SOUND_LABEL_BGM_sample002,		// BGM2
	SOUND_LABEL_BGM_sample003,		// BGM3
	SOUND_LABEL_BGM_sample004,		// BGM4
	SOUND_LABEL_BGM_sample005,		// BGM4
	SOUND_LABEL_BGM_selectscreen,	// BGM4
	SOUND_LABEL_SE_bomb000,			// ������
	SOUND_LABEL_SE_defend000,		// defend000��
	SOUND_LABEL_SE_defend001,		// defend0001��
	SOUND_LABEL_SE_hit000,			// hit000��
	SOUND_LABEL_SE_item,			// item��
	SOUND_LABEL_SE_lockon000,		// lockon000��
	SOUND_LABEL_SE_shot000,			// shot000��
	SOUND_LABEL_SE_shot001,			// shot001��
	SOUND_LABEL_SE_button,			// button��
	SOUND_LABEL_SE_door,			// door��
	SOUND_LABEL_SE_score,			// score��
	SOUND_LABEL_SE_enemylaser,		// enemylaser��
	SOUND_LABEL_SE_gamesave,		// gamesave��
	SOUND_LABEL_SE_gameload,		// gameload��
	SOUND_LABEL_SE_scorecounter,	// scorecounter��
	SOUND_LABEL_SE_gamelvlcomplete,	// gamelvlcomplete��
	SOUND_LABEL_SE_explosion,		// explosion��
	SOUND_LABEL_SE_chargeattack,	// chargeattack��
	SOUND_LABEL_SE_vortex,			// chargeattack��
	SOUND_LABEL_SE_thundercharge,	// thundercharge��
	SOUND_LABEL_SE_blast,			// blast��
	SOUND_LABEL_SE_recharge,		// recharge��
	SOUND_LABEL_SE_playerdead,		// playerdead��
	SOUND_LABEL_SE_systembreak,		// systembreak��
	SOUND_LABEL_SE_hpdrop,			// hpdrop��
	SOUND_LABEL_SE_hugeexplosion,	// hugexplosion��
	SOUND_LABEL_SE_stun01,			// stun01��
	SOUND_LABEL_SE_mousepop,		// mousepop��
	SOUND_LABEL_SE_mouseswip,		// mouseswip��
	SOUND_LABEL_SE_door01,			// door01��

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

