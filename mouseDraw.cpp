//=============================================================================
//

//
//=============================================================================
#include "bg.h"
#include "mouseDraw.h"
#include "input.h"
#include "sprite.h"
#include "cursor.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "mousePad.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(5)		// キャラサイズ（横）
#define TEXTURE_HEIGHT				(5)		// キャラサイズ（縦）
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *texturName[] = {
	"data/TEXTURE/MOUSE/drawPix_sample.png",
};

static BOOL	  isLoad = FALSE;				// 初期化を行ったかのフラグ
static MOUSEDRAW mouseDraw[MOUSEDRAW_MAX];	// マウス描画構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMouseDraw(void)
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


	// マウス描画構造体の初期化
	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		mouseDraw[i].use   = FALSE;
		mouseDraw[i].w     = TEXTURE_WIDTH;
		mouseDraw[i].h     = TEXTURE_HEIGHT;
		mouseDraw[i].pos   = D3DXVECTOR3(300, 300.0f, 0.0f);
		mouseDraw[i].texNo = 0;

	}
	
	isLoad = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMouseDraw(void)
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
void UpdateMouseDraw(void)
{
	MOUSEPAD *mousePad = GetMousePad();

	int mousedrawCount = 0;				// 処理したマウス描画の数

	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		// マウス描画処理
		if (mouseDraw[i].use == TRUE)	// マウス描画されてる？
		{	// Yes
			// マウス判定用パッドの数分当たり判定を行う
			for (int j = 0; j < MOUSEPAD_MAX; j++)
			{
				// フラグの立ってないマウス判定用パッドとの当たり判定をする
				if (mousePad[j].use == FALSE)
				{
					BOOL ans = CollisionBB(mouseDraw[i].pos, mouseDraw[i].w, mouseDraw[i].h,
						mousePad[j].pos, mousePad[j].w, mousePad[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						mousePad[j].use = TRUE;
						break;
					}
				}
			}
			mousedrawCount++;
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMouseDraw(void)
{
	BG *bg = GetBG();
	MOUSEPAD *mousePad = GetMousePad();

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

	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		if (mouseDraw[i].use == TRUE)		// このマウス描画が使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[mouseDraw[i].texNo]);

			//マウス描画の位置やテクスチャー座標を反映
			float px = mouseDraw[i].pos.x;	// マウス描画の表示位置X
			float py = mouseDraw[i].pos.y;	// マウス描画の表示位置Y
			float pw = mouseDraw[i].w;		// マウス描画の表示幅
			float ph = mouseDraw[i].h;		// マウス描画の表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// マウス描画構造体の先頭アドレスを取得
//=============================================================================
MOUSEDRAW *GetMouseDraw(void)
{
	return &mouseDraw[0];
}


//=============================================================================
// マウス描画軌跡の描画設定
//=============================================================================
void SetMouseDraw(D3DXVECTOR3 pos)
{
	// もしマウス描画が無かったら描画しない( =これ以上書かないって事 )
	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		if (mouseDraw[i].use == FALSE)		// 未使用状態のマウス描画を見つける
		{
			mouseDraw[i].use = TRUE;		// 使用状態へ変更する
			mouseDraw[i].pos = pos;			// 座標をセット
			PlaySound(SOUND_LABEL_SE_mousepop);
			return;							// １描画セットしたので終了する
		}
	}
}

