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

struct BUTTON
{
	D3DXVECTOR3		pos;		// ポリゴンの座標
	D3DXCOLOR		color;
	float			w, h;		// 幅と高さ
	int				texNo;		// 使用しているテクスチャ番号
	bool			use;
};

enum
{
	START_BUTTON,
	EXIT_BUTTON,
	PLAYER_BUTTON,
	TITLE_LOGO,
	BUTTON_MAX
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitButton(void);
void UninitButton(void);
void UpdateButton(void);
void DrawButton(void);

void GameStartAnimation(void);
void SelectMenu(void);


