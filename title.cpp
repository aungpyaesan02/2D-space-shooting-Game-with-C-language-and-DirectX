//=============================================================================
//
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(2)				// テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/BG.png",
	"data/TEXTURE/BG/Stars.png",
};


static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ
static TITLE	title[TEXTURE_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
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

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		// 変数の初期化
		title[i].w = TEXTURE_WIDTH;
		title[i].h = TEXTURE_HEIGHT;
		title[i].u = title[i].v = 0.0f;
		title[i].uw = title[i].vh = 1.0f;
		title[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		title[i].texNo = i;
	}

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	title[LAYER_2].v -= 0.005f;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
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

		// タイトルの背景を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[title[i].texNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(vertexBuffer,
				title[i].pos.x, title[i].pos.y, title[i].w, title[i].h,
				title[i].u, title[i].v, title[i].uw, title[i].vh,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}





