//=============================================================================
//
//
//=============================================================================
#include "result.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "score.h"
#include "file.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(22)				// テクスチャの数

#define RANKING_DW					(128.0f)
#define RANKING_DH					(64.0f)
#define RANKING_DX					(100.0f)

#define PRESSENTER_DW				(512.0f)
#define PRESSENTER_DH				(128.0f)
#define PRESSENTER_DX				(SCREEN_CENTER_X - PRESSENTER_DW * 0.5f)
#define PRESSENTER_DY				(420.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/BG.png",
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
	"data/TEXTURE/BG/ranking01.png",
	"data/TEXTURE/BG/ranking02.png",
	"data/TEXTURE/BG/ranking03.png",
	"data/TEXTURE/BG/ranking04.png",
	"data/TEXTURE/BG/ranking05.png",
	"data/TEXTURE/BG/ranking06.png",
	"data/TEXTURE/BG/ranking07.png",
	"data/TEXTURE/BG/ranking08.png",
	"data/TEXTURE/BG/ranking09.png",
	"data/TEXTURE/BG/ranking10.png",
	"data/TEXTURE/BG/pressenter.png",

};


static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ
static RESULT	result;

static int		resultScore = 0;
static int		resultHighscore[HIGHSCORE_MAX];
static float	count;				// 点滅用のカウント
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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


	// 変数の初期化
	result.w     = TEXTURE_WIDTH;
	result.h     = TEXTURE_HEIGHT;
	result.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	result.texNo = 0;

	count = 0.0f;
	
	// ハイスコアのロード
	LoadHighscore();

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{

	count += 0.05f;

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_TITLE);

		// SE
		PlaySound(SOUND_LABEL_SE_defend001);

	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TITLE);

		// SE
		PlaySound(SOUND_LABEL_SE_defend001);

	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE);

		// SE
		PlaySound(SOUND_LABEL_SE_defend001);

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[result.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			result.pos.x, result.pos.y, result.w, result.h, 
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 最終スコア表示
	{

		// 桁数分処理する
		int number = resultScore;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			int x = (number % 10);
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[x + 1]);

			// 次の桁へ
			number /= 10;

			// スコアの位置やテクスチャー座標を反映
			float px = SCORE_DX - SCORE_DW * i;	// スコアの表示位置X
			float py = SCORE_DY;				// スコアの表示位置Y
			float pw = SCORE_DW;				// スコアの表示幅
			float ph = SCORE_DH;				// スコアの表示高さ

			float tw = 1.0f / 1;				// テクスチャの幅
			float th = 1.0f / 1;				// テクスチャの高さ
			float tx = 0.0f;					// テクスチャの左上X座標
			float ty = 0.0f;					// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(vertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

	}

	// ハイスコア表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[1]);

		for (int j = 0; j < HIGHSCORE_MAX; j++)
		{
			// 桁数分処理する
			int number = resultHighscore[j];

			for (int i = 0; i < SCORE_DIGIT; i++)
			{
				// 今回表示する桁の数字
				int x = number % 10;

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[x + 1]);

				// 次の桁へ
				number /= 10;

				// スコアの位置やテクスチャー座標を反映
				float px = HIGHSCORE_DX - HIGHSCORE_DW * i + HIGHSCORE_LEN_X * (j / (HIGHSCORE_MAX / 2));	// スコアの表示位置X
				float py = HIGHSCORE_DY + HIGHSCORE_LEN_Y * (j % (HIGHSCORE_MAX / 2));				// スコアの表示位置Y
				float pw = HIGHSCORE_DW;				// スコアの表示幅
				float ph = HIGHSCORE_DH;				// スコアの表示高さ

				float tw = 1.0f;				// テクスチャの幅
				float th = 1.0f;				// テクスチャの高さ
				float tx = 0.0f;					// テクスチャの左上X座標
				float ty = 0.0f;					// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(vertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}


		}

	}

	// 順位の表示
	{


		for (int i = 0; i < HIGHSCORE_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[i + 11]);


			// 順位の位置やテクスチャー座標を反映
			float px = RANKING_DX + HIGHSCORE_LEN_X * (i / (HIGHSCORE_MAX / 2));	// 順位の表示位置X
			float py = HIGHSCORE_DY + HIGHSCORE_LEN_Y * (i % (HIGHSCORE_MAX / 2));				// 順位の表示位置Y
			float pw = RANKING_DW;					// 順位の表示幅
			float ph = RANKING_DH;					// 順位の表示高さ

			float tw = 1.0f;						// テクスチャの幅
			float th = 1.0f;						// テクスチャの高さ
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

	// PRESS ENTER の表示
	{

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[21]);


		// 位置やテクスチャー座標を反映
		float px = PRESSENTER_DX;					// 表示位置X
		float py = PRESSENTER_DY;					// 表示位置Y
		float pw = PRESSENTER_DW;					// 表示幅
		float ph = PRESSENTER_DH;					// 表示高さ

		float tw = 1.0f;						// テクスチャの幅
		float th = 1.0f;						// テクスチャの高さ
		float tx = 0.0f;						// テクスチャの左上X座標
		float ty = 0.0f;						// テクスチャの左上Y座標

		float a = 0.5f * (cosf(count) + 1.0f);
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, a));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}


}



//=============================================================================
// リザルトで表示する点をセット
//=============================================================================
void SetResult(int score)
{
	resultScore = score;

	// ハイスコアをロードする
	LoadHighscore();

	// ハイスコアの並び替え
	for (int i = 0; i < HIGHSCORE_MAX; i++)
	{
		if (score > resultHighscore[i])
		{

			for (int j = HIGHSCORE_MAX - 2; j > i - 1; j--)
			{
				resultHighscore[j + 1] = resultHighscore[j];
			}

			resultHighscore[i] = score;
			break;
		}
	}

	// SE
	PlaySound(SOUND_LABEL_SE_scorecounter);

}


int *GetHighScore(void)
{

	return &resultHighscore[0];
}