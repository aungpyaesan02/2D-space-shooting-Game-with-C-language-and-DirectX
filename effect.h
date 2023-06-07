//=============================================================================

//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define EFFECT_MAX			(64)		// エフェクトの最大数
#define EFFECT_DATA_MAX		(1)		// エフェクトの最大数

enum
{
	EFFECT_EXPLOSION,	// 爆発
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct EFFECT_DATA 
{
	int texNo;
	int texDivideX;
	int texDivideY;
};

struct EFFECT
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	D3DXVECTOR3		scl;			// ポリゴンの拡大率
	BOOL			use;			// true:使っている  false:未使用
	float			w, h;			// 幅と高さ
	int				countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	EFFECT_DATA*	data;
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

void SetEffect(int effectIndex, D3DXVECTOR3 pos, float w, float h);




