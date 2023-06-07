//=============================================================================
//
//
//=============================================================================
#include "vibration.h"
#include "player.h"
#include "sprite.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(800)	// 揺れ表示サイズ（横）
#define TEXTURE_HEIGHT				(800)	// 揺れ表示サイズ（縦）
#define TEXTURE_MAX					(1)		// テクスチャの数
#define ANIM_WAIT					(80)	// アニメーションの切り替わるWait値
#define VIB_ADDITION_X				(1.0f)	// 揺れ加算用（横）
#define VIB_ADDITION_Y				(5.0f)	// 揺れ加算用（縦）
#define VIB_MAX_X					(2.0f)	// 揺れ幅Max（横）
#define VIB_MAX_Y					(10.0f)	// 揺れ幅Max（縦）

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/EFFECT/vibration_Damage.png",
};


static BOOL			isLoad = FALSE;		// 初期化を行ったかのフラグ
static VIBRATION	vibration;			// 揺れの構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitVibration(void)
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
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;			// 4頂点分
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// 揺れの構造体の初期化
	vibration.use = FALSE ;							// 生きているかどうか
	vibration.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 中心点から表示
	vibration.w   = TEXTURE_WIDTH;					// 幅
	vibration.h   = TEXTURE_HEIGHT;					// 高さ
	vibration.texNo = 0;							// テクスチャ番号

	vibration.countAnim = 0;						// アニメカウンター(wait)

	isLoad = TRUE;									// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitVibration(void)
{
	if (isLoad == FALSE) return;			//初期化していない→終了処理もしない

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
void UpdateVibration(void)
{

		// 使っている揺れだけ処理をする
		if (vibration.use == TRUE)
		{
			// アニメーション
			vibration.countAnim++;

			if (vibration.countAnim > ANIM_WAIT)
			{
				vibration.countAnim = 0;
				vibration.use = FALSE;
			}

			vibration.pos.x += VIB_ADDITION_X;
			if (vibration.pos.x > VIB_MAX_X)
			{
				vibration.pos.x = 0.0f;
			}

			vibration.pos.y += VIB_ADDITION_Y;
			if (vibration.pos.y > VIB_MAX_Y)
			{
				vibration.pos.y = 0.0f;
			}
		}

//#ifdef _DEBUG
//		// デバッグ表示
//		PrintDebugProc("BulletNo%d  X:%f Y:%f\n", i, vibration.pos.x, vibration.pos.y);
//#endif
	
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawVibration(void)
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

	if (vibration.use == TRUE)		// この揺れが使われている？
	{								// Yes
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[vibration.texNo]);

		//揺れの位置やテクスチャー座標を反映
		float px = vibration.pos.x;				// 揺れの表示位置X
		float py = vibration.pos.y;				// 揺れの表示位置Y
		float pw = vibration.w;					// 揺れの表示幅
		float ph = vibration.h;					// 揺れの表示高さ


		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標			
		float ty = 0.0f;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// 揺れ構造体の先頭アドレスを取得
//=============================================================================
VIBRATION *GetVibration(void)
{
	return &vibration;
}


// 揺れをセット
void SetVibration()
{
	if (vibration.use == FALSE)
	{
		vibration.use = TRUE;
	}
}
