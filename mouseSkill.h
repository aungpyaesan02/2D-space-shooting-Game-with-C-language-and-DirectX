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
#define	MOUSESKILL_MAX			(1)			// マウススキルのMax数
#define	MOUSESKILL_DW			(100.0f)	// 表示サイズ（横）
#define	MOUSESKILL_DH			(100.0f)	// 表示サイズ（縦）

// マウススキル構造体
struct MOUSESKILL
{
	D3DXVECTOR3		pos;			// マウススキルの座標
	D3DXVECTOR3		rot;			// マウススキルの回転
	BOOL			use;			// このマウススキルが使われているか
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
	int				countAnim;		// アニメーションカウント
	//int				patternAnim;	// アニメーションパターンナンバー
	int				skillFlag;		// マウススキル選択用
	int				skillSwitch;	// マウススキルスイッチ用
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMouseSkill(void);
void UninitMouseSkill(void);
void UpdateMouseSkill(void);
void DrawMouseSkill(void);

MOUSESKILL *GetMouseSkill(void);
void SetMouseSkill(D3DXVECTOR3 pos, int skill);
void MouseSkillSelect(void);
