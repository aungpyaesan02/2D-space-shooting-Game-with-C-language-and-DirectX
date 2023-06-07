//=============================================================================
//

//
//=============================================================================
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(2)				// テクスチャの数

#define	FADE_RATE					(15.0f)			// フェード係数
#define FADE_MAX					(2)				// フェードの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void SetVertex(float X, float Y, float Width, float Height, float U, float V, float UW, float VH, D3DXCOLOR color);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[] = {
	"data/TEXTURE/BG/metalbridge_top.jpg",
	"data/TEXTURE/BG/metalbridge_bot.jpg",
};

static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ
static FADE		fade[FADE_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
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
	for (int i = 0; i < FADE_MAX; i++)
	{
		fade[i].w = TEXTURE_WIDTH;
		fade[i].h = TEXTURE_HEIGHT;
		fade[i].texNo = i;

		fade[i].flag = FADE_IN;
		fade[i].nextMode = MODE_END;
		fade[i].color = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
	}

	fade[0].pos = D3DXVECTOR3(0.0f, -SCREEN_HEIGHT, 0.0f);
	fade[1].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
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
void UpdateFade(void)
{
	const float offset = 0.1f;

	if (fade[0].flag != FADE_NONE)
	{// フェード処理中
		if (fade[0].flag == FADE_OUT)
		{// フェードアウト処理

			fade[0].pos.y += FADE_RATE;		// 上から下に行く
			fade[1].pos.y -= FADE_RATE;		// 下から上に行く

			// 真ん中にあると
			if ((fade[0].pos.y >= -SCREEN_HEIGHT * 0.5f + offset) && fade[1].pos.y <= SCREEN_HEIGHT * 0.5f - offset)
			{
				// フェードイン処理に切り替え
				fade[0].pos.y = -SCREEN_HEIGHT * 0.5f;
				fade[1].pos.y =  SCREEN_HEIGHT * 0.5f;

				Sleep(800);

				SetFade(FADE_IN, fade[0].nextMode);

				// モードを設定
				SetMode(fade[0].nextMode);
			}

			// SE
			PlaySound(SOUND_LABEL_SE_defend001);

		}
		else if (fade[0].flag == FADE_IN)
		{// フェードイン処理
			fade[0].pos.y -= FADE_RATE;		// 真ん中から上に行く
			fade[1].pos.y += FADE_RATE;		// 真ん中から下に行く

			if ((fade[0].pos.y <= -SCREEN_HEIGHT) && (fade[1].pos.y >= SCREEN_HEIGHT))
			{
				// フェード処理終了
				fade[0].pos.y = -SCREEN_HEIGHT;
				fade[1].pos.y =  SCREEN_HEIGHT;

				SetFade(FADE_NONE, fade[0].nextMode);
			}

			// SE
			PlaySound(SOUND_LABEL_SE_defend001);

		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (fade[0].flag == FADE_NONE) return;	// フェードしないのなら描画しない

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

	for (int i = 0; i < FADE_MAX; i++)
	{
		// フェード用ポリゴンを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[fade[i].texNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetVertex(fade[i].pos.x, fade[i].pos.y, fade[i].w, fade[i].h, 0.0f, 0.0f, 1.0f, 1.0f, fade[i].color);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}
}

//=============================================================================
// 頂点データ設定
//=============================================================================
static void SetVertex(float X, float Y, float Width, float Height, float U, float V, float UW, float VH, D3DXCOLOR color)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

	vertex[0].Position = D3DXVECTOR3(X, Y, 0.0f);
	vertex[0].Diffuse  = color;
	vertex[0].TexCoord = D3DXVECTOR2(U, V);

	vertex[1].Position = D3DXVECTOR3(X + Width, Y, 0.0f);
	vertex[1].Diffuse  = color;
	vertex[1].TexCoord = D3DXVECTOR2(U + UW, V);

	vertex[2].Position = D3DXVECTOR3(X, Y + Height, 0.0f);
	vertex[2].Diffuse  = color;
	vertex[2].TexCoord = D3DXVECTOR2(U, V + VH);

	vertex[3].Position = D3DXVECTOR3(X + Width, Y + Height, 0.0f);
	vertex[3].Diffuse  = color;
	vertex[3].TexCoord = D3DXVECTOR2(U + UW, V + VH);


	GetDeviceContext()->Unmap(vertexBuffer, 0);
}


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(int fadeFlg, int nextMode)
{
	fade[0].flag = fadeFlg;
	fade[0].nextMode = nextMode;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
int GetFade(void)
{
	return fade[0].flag;
}



