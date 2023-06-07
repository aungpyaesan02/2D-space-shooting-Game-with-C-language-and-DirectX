//=============================================================================
//

//
//=============================================================================
#include "bullet.h"
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "vibration.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(35)	// キャラサイズ
#define TEXTURE_HEIGHT				(35)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値

#define BULLET_SPEED				(3.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/ENEMY/tile_0000.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;						// 4頂点分
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE ;								// 生きているかどうか
		g_Bullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 中心点から表示
		g_Bullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// rotation
		g_Bullet[i].w   = TEXTURE_WIDTH;						// 幅
		g_Bullet[i].h   = TEXTURE_HEIGHT;						// 高さ
		g_Bullet[i].texNo = 0;									// テクスチャ番号

		g_Bullet[i].countAnim = 0;								// アニメカウンター(wait)
		g_Bullet[i].patternAnim = 0;							// アニメパターン番号

		g_Bullet[i].move = D3DXVECTOR3(0.0f, -BULLET_SPEED, 0.0f);		// 移動量

	}

	g_Load = TRUE;												// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;								//初期化していない→終了処理もしない

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

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		// 使っている弾だけ処理をする
		if (g_Bullet[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = g_Bullet[i].pos;

			//// アニメーション
			//g_Bullet[i].countAnim += 1.0f;

			//if (g_Bullet[i].countAnim > ANIM_WAIT)
			//{
			//	g_Bullet[i].countAnim = 0.0f;
			//	// パターンの切り替え(1ずつ大きくしておいてパターンの最大数になったらリセットする、これでループできる)
			//	g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			//}


			// 移動 
			{

				g_Bullet[i].pos += g_Bullet[i].move;


				// MAP外チェック
				BG *bg = GetBG();

				if (g_Bullet[i].pos.x + g_Bullet[i].w / 2 < 0.0f)
				{
					g_Bullet[i].use = FALSE;
				}

				if (g_Bullet[i].pos.x - g_Bullet[i].w / 2> bg->w)
				{
					g_Bullet[i].use = FALSE;
				}

				if (g_Bullet[i].pos.y + g_Bullet[i].h / 2 < 0.0f)
				{
					g_Bullet[i].use = FALSE;
				}

				if (g_Bullet[i].pos.y - g_Bullet[i].h / 2> bg->h)
				{
					g_Bullet[i].use = FALSE;
				}

				// 移動が終わったらプレイヤーとの当たり判定
				{
					PLAYER *p = GetPlayer();
					// シールドが発動している？
					if (p[0].shieldTime <= 0.0f)
					{// No
						BOOL ans = CollisionBB(p[0].pos, p[0].w, p[0].h, g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							p[0].hp -= DAMAGE;
							SetVibration();
							PlaySound(SOUND_LABEL_SE_hpdrop);

							g_Bullet[i].use = FALSE;

							if (p[0].hp <= 0)
							{
								p[0].use = FALSE;
								SetFade(FADE_OUT, MODE_RESULT);
							}

						}
					}

				}


			}
		}
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// この弾が使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[g_Bullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;	// 弾の表示位置X
			float py = g_Bullet[i].pos.y - bg->pos.y;	// 弾の表示位置Y
			float pw = g_Bullet[i].w;		// 弾の表示幅
			float ph = g_Bullet[i].h;		// 弾の表示高さ


			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標

			float ty = (g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{

	return &g_Bullet[0];
}



// 敵のバレットをセット
void SetBullet(D3DXVECTOR3 pos)
{
	PLAYER *p = GetPlayer();


	for (int i = 0; i < BULLET_MAX; i++)
	{

		if (g_Bullet[i].use == FALSE)
		{
			g_Bullet[i].pos = pos;
			g_Bullet[i].use = TRUE;

			D3DXVECTOR3 vec = p->pos - pos;

			float angle = atan2f(vec.y, vec.x);


			g_Bullet[i].rot.z = angle + D3DX_PI * 0.5f;
			g_Bullet[i].move.x = BULLET_SPEED * cosf(angle);
			g_Bullet[i].move.y = BULLET_SPEED * sinf(angle);
			// ここで効果音を鳴らす


			break;

		}
	}

}
