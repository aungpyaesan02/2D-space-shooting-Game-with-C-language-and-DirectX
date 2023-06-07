//=============================================================================
//
//
//=============================================================================
#include "pause.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PAUSE_MENU_WIDTH			(200)
#define PAUSE_MENU_HEIGHT			(PAUSE_MENU_WIDTH / 4)
#define PAUSE_MENU_OFFSET_Y			(50)

#define TEXTURE_WIDTH				(SCREEN_WIDTH)			// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)			// 
#define TEXTURE_MAX					(1 + PAUSE_MENU_MAX)	// テクスチャの数(背景＋メニュー)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/bg_pause.png",
	"data/TEXTURE/BG/pause_resume.png",
	"data/TEXTURE/BG/pause_restart.png",
	"data/TEXTURE/BG/pause_title.png",
};


static BOOL			isLoad = FALSE;		// 初期化を行ったかのフラグ
static PAUSE		pause;
static PAUSE_MENU	menu[PAUSE_MENU_MAX];

static int curMenuNum;					// 現在選択しているメニューの番号

static D3DXCOLOR defaultColor;			// 非選択時のメニュー色
static D3DXCOLOR selectedColor;			// 選択時のメニュー色
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPause(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			textureName[i],
			NULL,
			NULL,
			&texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// 変数の初期化
	pause.w     = TEXTURE_WIDTH;
	pause.h     = TEXTURE_HEIGHT;
	pause.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pause.texNo = 0;

	float leftX = SCREEN_CENTER_X - PAUSE_MENU_WIDTH * PAUSE_MENU_MAX * 0.5f;		// メニュー画像の左端座標
	for (int i = 0; i < PAUSE_MENU_MAX; i++)
	{
		menu[i].w = PAUSE_MENU_WIDTH;
		menu[i].h = PAUSE_MENU_HEIGHT;
		menu[i].pos = D3DXVECTOR3(leftX + PAUSE_MENU_WIDTH * i, SCREEN_CENTER_Y + PAUSE_MENU_OFFSET_Y, 0.0f);
		menu[i].texNo = i + 1;
		menu[i].selected = false;
	}

	curMenuNum = 0;
	menu[curMenuNum].selected = false;					// 最初に選択しておくメニュー

	defaultColor = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.7f);	// 選択していないときは暗めの色
	selectedColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	SetPause(false);	// ポーズ状態初期化

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPause(void)
{
	if (isLoad == FALSE) return;

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (texture[i])
		{
			texture[i]->Release();
			texture[i] = NULL;
		}
	}

	isLoad = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePause(void)
{
	// F1キーでポーズ状態切り替え
	if (GetKeyboardTrigger(DIK_F1))
	{
		SetPause(!GetPause());
		// SE
		PlaySound(SOUND_LABEL_SE_button);
	}

	// メニュー選択
	if (GetKeyboardTrigger(DIK_LEFTARROW))
	{
		curMenuNum--;
		// SE
		PlaySound(SOUND_LABEL_SE_button);

	}
	else if (GetKeyboardTrigger(DIK_RIGHTARROW))
	{
		curMenuNum++;
		// SE
		PlaySound(SOUND_LABEL_SE_button);

	}

	// 端まで行ったら反対側へ
	if (curMenuNum < 0)
	{
		curMenuNum = PAUSE_MENU_MAX - 1;
	}
	else if (curMenuNum >= PAUSE_MENU_MAX)
	{
		curMenuNum = 0;
	}

	// いったん全部選択解除
	for (int i = 0; i < PAUSE_MENU_MAX; i++)
	{
		menu[i].selected = false;
	}

	// 選択
	menu[curMenuNum].selected = true;

	// ポーズ中にエンターキーを押した時
	if (GetKeyboardTrigger(DIK_RETURN) && GetPause() == TRUE)
	{
		switch (curMenuNum)
		{
		case PAUSE_MENU_RESUME:		// 続ける
			SetPause(false);
			break;

		case PAUSE_MENU_RESTART:	// リスタート
			// リスタート処理
			break;

		case PAUSE_MENU_TITLE:		// タイトル
			SetFade(FADE_OUT, MODE_TITLE);
			break;
		}

		PlaySound(SOUND_LABEL_SE_button);

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPause(void)
{
	// ポーズ中じゃなければ描画しない
	if (GetPause() == FALSE) return;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// ポーズ画面描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			pause.pos.x, pause.pos.y, pause.w, pause.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// ポーズメニュー描画
	for (size_t i = 0; i < PAUSE_MENU_MAX; i++)
	{
		// 選択しているかどうかで色を変更
		D3DXCOLOR color;
		menu[i].selected ? color = selectedColor : color = defaultColor;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[i + 1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			menu[i].pos.x, menu[i].pos.y, menu[i].w, menu[i].h,
			0.0f, 0.0f, 1.0f, 1.0f,
			color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}
