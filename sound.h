//=============================================================================
//
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
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
	SOUND_LABEL_SE_bomb000,			// 爆発音
	SOUND_LABEL_SE_defend000,		// defend000音
	SOUND_LABEL_SE_defend001,		// defend0001音
	SOUND_LABEL_SE_hit000,			// hit000音
	SOUND_LABEL_SE_item,			// item音
	SOUND_LABEL_SE_lockon000,		// lockon000音
	SOUND_LABEL_SE_shot000,			// shot000音
	SOUND_LABEL_SE_shot001,			// shot001音
	SOUND_LABEL_SE_button,			// button音
	SOUND_LABEL_SE_door,			// door音
	SOUND_LABEL_SE_score,			// score音
	SOUND_LABEL_SE_enemylaser,		// enemylaser音
	SOUND_LABEL_SE_gamesave,		// gamesave音
	SOUND_LABEL_SE_gameload,		// gameload音
	SOUND_LABEL_SE_scorecounter,	// scorecounter音
	SOUND_LABEL_SE_gamelvlcomplete,	// gamelvlcomplete音
	SOUND_LABEL_SE_explosion,		// explosion音
	SOUND_LABEL_SE_chargeattack,	// chargeattack音
	SOUND_LABEL_SE_vortex,			// chargeattack音
	SOUND_LABEL_SE_thundercharge,	// thundercharge音
	SOUND_LABEL_SE_blast,			// blast音
	SOUND_LABEL_SE_recharge,		// recharge音
	SOUND_LABEL_SE_playerdead,		// playerdead音
	SOUND_LABEL_SE_systembreak,		// systembreak音
	SOUND_LABEL_SE_hpdrop,			// hpdrop音
	SOUND_LABEL_SE_hugeexplosion,	// hugexplosion音
	SOUND_LABEL_SE_stun01,			// stun01音
	SOUND_LABEL_SE_mousepop,		// mousepop音
	SOUND_LABEL_SE_mouseswip,		// mouseswip音
	SOUND_LABEL_SE_door01,			// door01音

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

