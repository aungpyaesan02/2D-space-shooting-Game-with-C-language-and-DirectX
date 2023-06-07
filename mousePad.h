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
#define	MOUSEPAD_MAX		(9)			// マウス判定用パッドのMax数
#define	MOUSEPAD_DW			(40.0f)		// 表示サイズ（横）
#define	MOUSEPAD_DH			(40.0f)		// 表示サイズ（縦）
#define	MOUSEPAD_DX			(850.0f)	// 表示位置（横）
#define	MOUSEPAD_DY			(420.0f)	// 表示位置（縦）
#define	MOUSEPAD_DX1		(70.0f)		// 別個配置用位置1（横）
#define	MOUSEPAD_DY1		(70.0f)		// 別個配置用位置1（縦）
#define	MOUSEPAD_DX2		(35.0f)		// 別個配置用位置2（横）
#define	MOUSEPAD_DY2		(35.0f)		// 別個配置用位置2（縦）

// マウス判定用パッド構造体
struct MOUSEPAD
{
	D3DXVECTOR3		pos;		// ポリゴンの座標
	D3DXVECTOR3		rot;		// ポリゴンの座標
	BOOL			use;		// 使用未使用
	float			w, h;		// 幅と高さ
	int				texNo;		// 使用しているテクスチャ番号
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMousePad(void);
void UninitMousePad(void);
void UpdateMousePad(void);
void DrawMousePad(void);

MOUSEPAD *GetMousePad(void);

