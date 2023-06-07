//=============================================================================
//
//
//=============================================================================
#include "score.h"
#include "sprite.h"
#include "file.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCORE_DW)	// 数字のサイズ
#define TEXTURE_HEIGHT				(SCORE_DH)	// 
#define TEXTURE_MAX					(10)			// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *texturName[] = {
	"data/TEXTURE/BG/tile_0019.png",
	"data/TEXTURE/BG/tile_0020.png",
	"data/TEXTURE/BG/tile_0021.png",
	"data/TEXTURE/BG/tile_0022.png",
	"data/TEXTURE/BG/tile_0023.png",
	"data/TEXTURE/BG/tile_0031.png",
	"data/TEXTURE/BG/tile_0032.png",
	"data/TEXTURE/BG/tile_0033.png",
	"data/TEXTURE/BG/tile_0034.png",
	"data/TEXTURE/BG/tile_0035.png",
};

static BOOL		load = FALSE;		// 初期化を行ったかのフラグ
static SCORE	Score;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
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


	// 変数の初期化
	Score.w = TEXTURE_WIDTH;
	Score.h = TEXTURE_HEIGHT;
	Score.pos = D3DXVECTOR3(SCORE_DX, SCORE_DY, 0.0f);
	Score.texNo = 0;
	Score.score = 0;	// 得点を初期化
	

	load = TRUE;		// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	if (load == FALSE) return;

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

	load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[Score.texNo]);

	// 桁数分処理する
	int number = Score.score;

	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// 今回表示する桁の数字
		int x = (number % 10);
		Score.texNo = x;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[Score.texNo]);

		// 次の桁へ
		number /= 10;
		
		// スコアの位置やテクスチャー座標を反映
		float px = Score.pos.x - Score.w*i;	// スコアの表示位置X
		float py = Score.pos.y;				// スコアの表示位置Y
		float pw = Score.w;					// スコアの表示幅
		float ph = Score.h;					// スコアの表示高さ

		float tw = 1.0f / 1;					// テクスチャの幅
		float th = 1.0f / 1;					// テクスチャの高さ
		float tx = 0.0f;						// テクスチャの左上X座標
		float ty = 0.0f;						// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

}


//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddScore(int add)
{
	Score.score += add;
	if (Score.score > SCORE_MAX)
	{
		Score.score = SCORE_MAX;
	}

	// SE
	PlaySound(SOUND_LABEL_SE_score);

}


int GetScore(void)
{
	return Score.score;
}


void SetScore(int score)
{
	Score.score = score;
}

