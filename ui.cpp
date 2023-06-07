//=============================================================================
//
//
//=============================================================================
#include "sprite.h"
#include "player.h"
#include "UI.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	BAR_WIDTH					(100)	// タイトル画面にあるプレイヤーのサイズ
#define	BAR_HEIGHT					(15)	// タイトル画面にあるプレイヤーのサイズ
#define	ICON_WIDTH					(25)	// ボタンのサイズ
#define	ICON_HEIGHT					(25)	// ボタンのサイズ

#define TEXTURE_MAX					(2)		// テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/HEALTH_UI/healthIcon.png",
	"data/TEXTURE/HEALTH_UI/bar.png",
};

static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ
static UI		ui[UI_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
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

	//UIの初期化
	for (int i = 0; i < UI_MAX; i++)
	{
		// アイコンの初期化
		ui[ICON].w = ICON_WIDTH;
		ui[ICON].h = ICON_HEIGHT;
		ui[ICON].pos = { 10.0f, SCREEN_HEIGHT - 30.0f, 0.0f };

		// バーの初期化
		ui[BAR].w = BAR_WIDTH;
		ui[BAR].h = BAR_HEIGHT;
		ui[BAR].pos = { ui[ICON].pos.x + 25.0f, SCREEN_HEIGHT - 25.0f, 0.0f };

		ui[i].color = { 1.0f,1.0f,1.0f,1.0f };
		ui[i].texNo = i;
		
		ui[i].use = true;
	}

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	PLAYER* player = GetPlayer();
	
	// プレイヤーHPによりUI（バー）の長さがかわる
	ui[BAR].w = player->hp * 10;

	if (ui[BAR].w < 0.0f)
		ui[BAR].w = 0.0f;

#ifdef _DEBUG

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (ui[i].use)
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
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[ui[i].texNo]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(vertexBuffer,
					ui[i].pos.x, ui[i].pos.y, ui[i].w, ui[i].h,
					0.0f, 0.0f, 1.0f, 1.0f,
					ui[i].color);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
}






