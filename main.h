//=============================================================================

//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>
#include <time.h>

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)


#include "debugproc.h"


//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH	(960)				// ウインドウの幅
#define SCREEN_HEIGHT	(540)				// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// ウインドウの中心Ｙ座標

#define	NUM_VERTEX		(4)					// 頂点数

enum
{
	MODE_TITLE = 0,							// タイトル画面
	MODE_TUTORIAL,							// ゲーム説明画面
	MODE_STAGE_SELECT,
	MODE_GAME,								// ゲーム画面
	MODE_RESULT,							// リザルト画面
	MODE_END
};

// 補間用のデータ構造体を定義
struct INTERPOLATION_DATA
{
	D3DXVECTOR3	pos;		// 頂点座標
	D3DXVECTOR3	rot;		// 回転
	D3DXVECTOR3	scl;		// 拡大縮小
	float		frame;		// 実行フレーム数 ( dt = 1.0f/frame )
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char *GetDebugStr(void);

void SetMode(int mode);
int GetMode(void);


BOOL GetPause(void);
void SetPause(BOOL flg);