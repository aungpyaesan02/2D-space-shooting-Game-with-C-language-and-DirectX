//=============================================================================
//
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_MAX			(99999)			// スコアの最大値
#define SCORE_DIGIT			(5)				// 桁数
#define HIGHSCORE_MAX		(10)			// セーブするハイスコアの数

#define	SCORE_DX			(540.0f)		// 表示位置
#define	SCORE_DY			(20.0f)			// 
#define	SCORE_DW			(32.0f)			// 
#define	SCORE_DH			(64.0f)			// 

#define	HIGHSCORE_DX			(400.0f)	// 表示位置
#define	HIGHSCORE_DY			(100.0f)	// 
#define	HIGHSCORE_DW			(32.0f)		// 
#define	HIGHSCORE_DH			(64.0f)		// 

#define HIGHSCORE_LEN_X			(360)	// ハイスコアを並べる間隔
#define HIGHSCORE_LEN_Y			(60)

#define ENEMY_DESTORY_SCORE		(120)	// エネミーを倒した時のスコア加算値

struct SCORE
{
	D3DXVECTOR3		pos;					// ポリゴンの座標
	float			w, h;					// 幅と高さ
	int				texNo;					// 使用しているテクスチャ番号
	int				score;					// 得点
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

int GetScore(void);
void AddScore(int add);
void SetScore(int score);


