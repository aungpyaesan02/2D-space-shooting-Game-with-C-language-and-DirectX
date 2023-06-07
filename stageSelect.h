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
#define STAGESELECT_MAX		(9)				// ステージセレクトのMax数
#define ANIM_MAX			(5)				// セレクトアニメーションのMax数
#define DOOR_MAX			(5)				// ドアアニメーションのMax数

enum
{
	CHAR_SELECT_STAGE_1,	// ステージ１
	CHAR_SELECT_STAGE_2,	// ステージ２
	CHAR_SELECT_STAGE_3,	// ステージ３
	CHAR_SELECT_STAGE_4,	// ステージ４
	CHAR_SELECT_STAGE_B, 	// ステージボス
	CHAR_SELECT_STAGE_NONE 	// ステージノーセレクト

};

// ステージセレクトの構造体
struct STAGESELECT
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
};

// セレクト用フレーム構造体
struct STAGESELECT_FLAME
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	int				flameSw;		// フレーム切り替え用
};

// セレクトアニメーション用構造体
struct STAGESELECT_ANIM
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	BOOL			selectNo;		// どのステージをセレクトしているかでアニメーションを切り替える（enum参照）
	char			flameFlag;		// 選択用フレームがあるかないかのフラグ（Y:ある N:ない）
};

// 開くドアアニメーション用構造体
struct STAGESELECT_DOOR
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	char			flameFlag;		// 選択用フレームがあるかないかのフラグ（Y:ある N:ない）
};

// 準備画面用構造体
struct COMINGSOON
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	char			useFlag;		// 選択用フレームがあるかないかのフラグ（Y:ある N:ない）
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStageSelect(void);
void UninitStageSelect(void);
void UpdateStageSelect(void);
void DrawStageSelect(void);
