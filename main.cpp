//=============================================================================
//

//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "title.h"
#include "button.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "item.h"
#include "result.h"
#include "sound.h"
#include "fade.h"
#include "file.h"
#include "pause.h"
#include "effect.h"
#include "bullet.h"
#include "stageSelect.h"

#include "cursor.h"
#include "mousePad.h"
#include "mouseDraw.h"
#include "mouseSkill.h"
#include "vibration.h"
#include "tutorial.h"
#include "ui.h"
#include "playerBullet.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME			"AppClass"					// ウインドウのクラス名
#define WINDOW_NAME			"Stellar Draw Shooter"	// ウインドウのキャプション名

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
void CheckGameClear(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long mousePosX = 0;
long mousePosY = 0;


#ifdef _DEBUG
int		cntFPS;							// FPSカウンタ
char	debugStr[2048] = WINDOW_NAME;	// デバッグ文字表示用

#endif

int gameMode = MODE_TITLE;				// 起動時の画面を設定


BOOL isPause = FALSE;					// ポーズ中ならtrue

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	//UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// ウィンドウの左座標
						CW_USEDEFAULT,																		// ウィンドウの上座標
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// ウィンドウ横幅
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
						NULL,
						NULL,
						hInstance,
						NULL);

	// ウィンドウモードかフルスクリーンモードかの処理
	bool mode = true;

	int id = MessageBox(NULL, "Windowモードでプレイしますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// YesならWindowモードで起動
		mode = true;
		break;
	case IDNO:		// Noならフルスクリーンモードで起動
		mode = false;
		break;
	case IDCANCEL:	// CANCELなら終了
	default:
		return -1;
		break;
	}

	// DirectXの初期化(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;
	
	// ウインドウの表示(Init()の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// システム時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				cntFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(debugStr, WINDOW_NAME);
				wsprintf(&debugStr[strlen(debugStr)], " FPS:%d", cntFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&debugStr[strlen(debugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, debugStr);
#endif

				dwFrameCount++;		// 処理回数のカウントを加算
			}
		}
	}
	
	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:					// [ESC]キーが押された
			DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する
			break;
		}
		break;

	case WM_MOUSEMOVE:
		mousePosX = LOWORD(lParam);
		mousePosY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// ランダムの初期化
	srand((unsigned int)time(NULL));

	// レンダリング処理の初期化
	InitRenderer(hInstance, hWnd, bWindow);

	// カメラ処理の初期化
	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンド処理の初期化
	InitSound(hWnd);

	// フェード処理の初期化
	InitFade();

	// 最初のモードをセットして、この中で初期化する
	SetMode(gameMode);

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_END);

	// フェードの終了処理
	UninitFade();

	// サウンドの終了処理
	UninitSound();

	// 入力の終了処理
	UninitInput();

	// カメラの終了処理
	UninitCamera();

	// レンダリングの終了処理
	UninitRenderer();

}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラの更新処理
	UpdateCamera();

	// モードによって処理を分ける
	switch (gameMode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		UpdateButton();
		break;

	case MODE_TUTORIAL:
		UpdateTitle();
		UpdateTutorial();
		break;

	case MODE_STAGE_SELECT: // ステージセレクト画面の更新
		UpdateStageSelect();
		break;

	case MODE_GAME:			// ゲーム画面の更新
		UpdatePause();

		if (GetPause() == FALSE)	// ポーズ状態じゃない場合のみ更新
		{
			UpdateBG();
			UpdatePlayer();
			UpdateEnemy();
			UpdateBullet();
			UpdateScore();
			UpdateEffect();
			UpdateItem();

			UpdateMousePad();
			UpdateCursor();
			UpdateMouseDraw();
			UpdateMouseSkill();
			UpdateVibration();
			UpdateUI();
			UpdatePlayerBullet();

			CheckGameClear();	// ゲームクリアかチェックする
		}

		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;

	}

	UpdateFade();			// フェードの更新処理
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	// カメラをセット
	SetCamera();

	// 2D描画なので深度無効
	SetDepthEnable(false);
	
	// モードによって処理を分ける
	switch (gameMode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		DrawTitle();
		DrawButton();
		break;

	case MODE_TUTORIAL:
		DrawTitle();
		DrawTutorial();
		break;

	case MODE_STAGE_SELECT: // ステージセレクト画面の描画
		DrawStageSelect();
		break;

	case MODE_GAME:			// ゲーム画面の描画
		DrawBG();
		DrawItem();
		DrawEnemy();
		DrawPlayerBullet();
		DrawPlayer();
		DrawPause();
		DrawBullet();
		DrawScore();
		DrawEffect();

		DrawMousePad();
		DrawMouseDraw();
		DrawMouseSkill();
		DrawCursor();
		DrawVibration();
		DrawUI();
		
		break;

	case MODE_RESULT:		// リザルト画面の描画
		DrawResult();
		break;
	}

	DrawFade();				// フェード画面の描画


#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// 曲を止める
	StopSound();

	// モードを変える前にメモリをすべて開放
	UninitTitle();
	UninitTutorial();
	UninitItem();
	UninitStageSelect();
	UninitButton();
	UninitBG();
	UninitPlayer();
	UninitEnemy();
	UninitBullet();
	UninitPause();
	UninitEffect();
	UninitResult();
	UninitScore();
	UninitEffect();
	UninitMousePad();
	UninitCursor();
	UninitMouseDraw();
	UninitMouseSkill();
	UninitVibration();
	UninitUI();
	UninitPlayerBullet();

	gameMode = mode;	// 次のモードをセット

	switch (gameMode)
	{
	case MODE_TITLE:		// タイトル画面
		InitTitle();
		InitButton();
		
		// BGM
		PlaySound(SOUND_LABEL_BGM_sample003);
		break;

	case MODE_TUTORIAL:
		InitTitle();
		InitTutorial();
		// BGM
		PlaySound(SOUND_LABEL_BGM_sample000); // チュートリアル用のBGM
		break;

	case MODE_STAGE_SELECT: // ステージセレクト画面
		InitStageSelect();
		
		// BGM
		PlaySound(SOUND_LABEL_BGM_selectscreen);		
		break;
	case MODE_GAME:			// ゲーム画面
		InitBG();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitPause();
		InitScore();
		InitEffect();
		InitItem();
		InitMousePad();
		InitCursor();
		InitMouseDraw();
		InitMouseSkill();
		InitVibration();
		InitUI();
		InitPlayerBullet();

		//// BGM
		PlaySound(SOUND_LABEL_BGM_sample005);
		break;

	case MODE_RESULT:		// リザルト画面
		InitResult();
		// BGM
		PlaySound(SOUND_LABEL_BGM_sample004);
		break;

	case MODE_END:			// ゲーム終了
		StopSound();
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return gameMode;
}


//=============================================================================
// マウス座標の取得
//=============================================================================
long GetMousePosX(void)
{
	return mousePosX;
}

long GetMousePosY(void)
{
	return mousePosY;
}


#ifdef _DEBUG
char *GetDebugStr(void)
{
	return debugStr;
}
#endif


//=============================================================================
// ゲームクリアかチェックする
//=============================================================================
void CheckGameClear(void)
{
	if (GetKeyboardTrigger(DIK_F3))
	{
		// リザルトにスコアをセットする
		SetResult(GetScore());
		SaveHighscore();
		SetFade(FADE_OUT, MODE_RESULT);	// リザルト画面へ
	}
}


//=============================================================================
// ゲームのポーズ状態を取得・設定する
//=============================================================================
BOOL GetPause(void)
{
	return isPause;
}

void SetPause(BOOL flg)
{
	isPause = flg;
}

