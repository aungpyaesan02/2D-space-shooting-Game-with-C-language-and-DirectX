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
#define PLAYER_MAX					(1)		// プレイヤーのMax人数

#define DAMAGE						(1)		// 受けた攻撃の値


enum
{
	CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,
	CHAR_DIR_RIGHT,
	CHAR_DIR_UP,
	CHAR_DIR_MAX
};

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	BOOL			use;			// true:使っている  false:未使用
	float			w, h;			// 幅と高さ
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	int				texNo;			// テクスチャ番号
	int				hp;				// ヒットポイント
	D3DXVECTOR3		move;			// 移動速度

	int				dir;			// 向き（詳細はplayer.h内enumを参照）
	BOOL			moving;			// Key押されているかフラグ


	float			shieldTime;		// シールド発動時間
	float			speedUpTime;	// 速度増加時間
	float			bulletSpeedUpTime;	// 速度増加時間
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void SetPlayerHP(int hp);
void SetPlayerShield(float shieldTime);
void SetPlayerMoveSpeed(float moveSpeedUp);
void SetPlayerBulletSpeed(float bulletSpeedUp);



