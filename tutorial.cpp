//=============================================================================
//

//
//=============================================================================
#include "tutorial.h"
#include "sprite.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(840)		// 数字のサイズ
#define TEXTURE_HEIGHT				(480) 
#define TEXTURE_MAX					(1)			// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/tutorial.jpg"
};

static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
TUTORIAL		tutorial;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 変数の初期化
	tutorial.w = TEXTURE_WIDTH;
	tutorial.h = TEXTURE_HEIGHT;
	tutorial.texNo = 0;

	tutorial.use = TRUE;

	tutorial.pos = D3DXVECTOR3(60.0f, 30.0f, 0.0f);

	g_Load = TRUE;		// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
	if (tutorial.use == TRUE)
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{			
			//PlaySound(SOUND_LABEL_SE_click);	←サウンドファイルが無かったので適当な奴に差し替えました　小峰
			PlaySound(SOUND_LABEL_SE_button);

			SetFade(FADE_OUT, MODE_STAGE_SELECT);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
{
	if (tutorial.use == TRUE)
	{
		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// マトリクス設定
		SetWorldViewProjection2D();

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// マテリアル設定
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tutorial.texNo]);

		// バトルログの位置やテクスチャー座標を反映
		float px = tutorial.pos.x;				// バトルログの表示位置X
		float py = tutorial.pos.y;				// バトルログの表示位置Y
		float pw = tutorial.w;					// バトルログの表示幅
		float ph = tutorial.h;					// バトルログの表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}

TUTORIAL *GetTutorial(void)
{
	return &tutorial;
}

