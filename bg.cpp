//=============================================================================
//
//
//=============================================================================
#include "bg.h"
#include "sprite.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(2048.0f)	// 背景サイズ
#define TEXTURE_HEIGHT				(2048.0f)	// 背景サイズ
#define TEXTURE_MAX					(1)				// テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/bg_stage01.png",
};


static BOOL	isLoad = FALSE;		// 初期化を行ったかのフラグ
static BG	bg;					// BG構造体

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
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
	bg.w     = TEXTURE_WIDTH;
	bg.h     = TEXTURE_HEIGHT;
	bg.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bg.texNo = 0;

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	// マップスクロール
	PLAYER *player = GetPlayer();

	bg.pos.x = player[0].pos.x - PLAYER_DISP_X;
	if (bg.pos.x < 0) bg.pos.x = 0;
	if (bg.pos.x > (bg.w - SCREEN_WIDTH)) bg.pos.x = bg.w - SCREEN_WIDTH;

	bg.pos.y = player[0].pos.y - PLAYER_DISP_Y;
	if (bg.pos.y < 0) bg.pos.y = 0;
	if (bg.pos.y > bg.h - SCREEN_HEIGHT) bg.pos.y = bg.h - SCREEN_HEIGHT;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
{
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

	// BGの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[bg.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			-bg.pos.x, -bg.pos.y, bg.w, bg.h, 
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG *GetBG(void)
{
	return &bg;
}