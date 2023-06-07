//=============================================================================

//
//=============================================================================
#include "effect.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "player.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[EFFECT_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[EFFECT_MAX] = {
	"data/TEXTURE/EFFECT/effect_explosion.png",
};


static BOOL			isLoad = FALSE;			// 初期化を行ったかのフラグ
static EFFECT		effect[EFFECT_MAX];		// エフェクト構造体
static EFFECT_DATA	effectData[EFFECT_DATA_MAX]{
	// tex, divideX, divideY
	{ 0, 5, 1 },	// EXPLOSION
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < EFFECT_MAX; i++)
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


	// エフェクト構造体の初期化
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		effect[i].use = FALSE;
		effect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		effect[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		effect[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		effect[i].countAnim = 0;
		effect[i].patternAnim = 0;
	}

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
{
	if (isLoad == FALSE) return;

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	for (int i = 0; i < EFFECT_MAX; i++)
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
void UpdateEffect(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_F2))
	{
		SetEffect(EFFECT_EXPLOSION, D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f), 100, 100);
	}
#endif // DEBUG


	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == TRUE)
		{
			// アニメーション  
			effect[i].countAnim++;
			if (effect[i].countAnim >= ANIM_WAIT)
			{
				effect[i].countAnim = 0;

				// パターンの切り替え
				effect[i].patternAnim++;
				int patternMax = effect[i].data->texDivideX * effect[i].data->texDivideY;

				// 再生が終わったら自動で消える
				if (effect[i].patternAnim >= patternMax)
				{
					effect[i].use = false;
				}
			}
		}
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(void)
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

	BG *bg = GetBG();

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == TRUE)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[effect[i].data->texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = effect[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = effect[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = effect[i].w;					// エネミーの表示幅
			float ph = effect[i].h;					// エネミーの表示高さ

			float tw = 1.0f / effect[i].data->texDivideX;	// テクスチャの幅
			float th = 1.0f / effect[i].data->texDivideY;	// テクスチャの高さ
			float tx = (float)(effect[i].patternAnim % effect[i].data->texDivideX) * tw;	// テクスチャの左上X座標
			float ty = (float)(effect[i].patternAnim / effect[i].data->texDivideY) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				effect[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// エネミー構造体の先頭アドレスを取得
//=============================================================================
void SetEffect(int effectIndex, D3DXVECTOR3 pos, float w, float h)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == FALSE)
		{
			effect[i].use = TRUE;
			effect[i].pos = pos;
			effect[i].w = w;
			effect[i].h = h;
			effect[i].countAnim = 0;
			effect[i].patternAnim = 0;
			effect[i].data = &effectData[effectIndex];
			break;
		}
	}

}




