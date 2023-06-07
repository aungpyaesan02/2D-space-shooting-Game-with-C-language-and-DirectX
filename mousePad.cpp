//=============================================================================
//
//
//=============================================================================
#include "bg.h"
#include "mousePad.h"
#include "mouseDraw.h"
#include "sprite.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(MOUSEPAD_DW)		// マウス判定用パッドのサイズ（横）
#define TEXTURE_HEIGHT				(MOUSEPAD_DH)		// マウス判定用パッドのサイズ（縦）
#define TEXTURE_MAX					(1)					// テクスチャの数
#define MOUSEPAD_INCLINATION		(0.785f)			// マウス判定用パッドの傾き


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *texturName[TEXTURE_MAX] = {
	"data/TEXTURE/MOUSE/mousePad_sample.png",

};


static BOOL	isLoad = FALSE;					// 初期化を行ったかのフラグ
static MOUSEPAD	mousePad[MOUSEPAD_MAX];		// マウス判定用パッド構造体


// 初期化処理
//=============================================================================
HRESULT InitMousePad(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			texturName[i],
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

	for (int i = 0; MOUSEPAD_MAX > i; i++)
	{
		// 変数の初期化
		mousePad[i].use = FALSE;
		mousePad[i].w = TEXTURE_WIDTH;
		mousePad[i].h = TEXTURE_HEIGHT;
		mousePad[i].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY, 0.0f);
		mousePad[i].rot = D3DXVECTOR3(0.0f, 0.0f, MOUSEPAD_INCLINATION); // 斜めにする
		mousePad[i].texNo = 0;

	}

	// マウス判定用パッドを別個で配置

	/*
			／0＼
		 ／1＼ ／2＼	
	  ／3＼ ／4＼ ／5＼
	  ＼ ／6＼ ／7＼ ／
		 ＼ ／8＼ ／
		    ＼ ／
	*/

	// ０
	mousePad[0].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY - MOUSEPAD_DY1, 0.0f);
	// １
	mousePad[1].pos = D3DXVECTOR3(MOUSEPAD_DX - MOUSEPAD_DX2, MOUSEPAD_DY - MOUSEPAD_DY2, 0.0f);
	// ２
	mousePad[2].pos = D3DXVECTOR3(MOUSEPAD_DX + MOUSEPAD_DX2, MOUSEPAD_DY - MOUSEPAD_DY2, 0.0f);
	// ３
	mousePad[3].pos = D3DXVECTOR3(MOUSEPAD_DX - MOUSEPAD_DX1, MOUSEPAD_DY, 0.0f);
	// ４
	mousePad[4].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY, 0.0f);
	// ５
	mousePad[5].pos = D3DXVECTOR3(MOUSEPAD_DX + MOUSEPAD_DX1, MOUSEPAD_DY, 0.0f);
	// ６
	mousePad[6].pos = D3DXVECTOR3(MOUSEPAD_DX - MOUSEPAD_DX2, MOUSEPAD_DY + MOUSEPAD_DY2, 0.0f);
	// ７
	mousePad[7].pos = D3DXVECTOR3(MOUSEPAD_DX + MOUSEPAD_DX2, MOUSEPAD_DY + MOUSEPAD_DY2, 0.0f);
	// ９
	mousePad[8].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY + MOUSEPAD_DY1, 0.0f);

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMousePad(void)
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
void UpdateMousePad(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMousePad(void)
{
	BG *bg = GetBG();

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

	for (int i = 0; MOUSEPAD_MAX > i; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[mousePad[i].texNo]);

		// マウス判定用パッドの背景を描画
		{
			// スコアの位置やテクスチャー座標を反映
			float px = mousePad[i].pos.x;	// マウス判定用パッドの表示位置X
			float py = mousePad[i].pos.y;	// マウス判定用パッドの表示位置Y
			float pw = mousePad[i].w;		// マウス判定用パッドの表示幅
			float ph = mousePad[i].h;		// マウス判定用パッドの表示高さ

			float tw = 1.0f;		// テクスチャの幅
			float th = 1.0f;		// テクスチャの高さ
			float tx = 0.0f;		// テクスチャの左上X座標
			float ty = 0.0f;		// テクスチャの左上Y座標

			// マウス描画と当たっていない時は原色
			if (mousePad[i].use == FALSE)
			{
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					mousePad[i].rot.z);
			}

			// マウス描画と当たっている時は色を変える
			if (mousePad[i].use == TRUE)
			{
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
					mousePad[i].rot.z);
			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// 構造体の先頭アドレスを取得
//=============================================================================
MOUSEPAD *GetMousePad(void)
{
	return &mousePad[0];
}





