//=============================================================================
//
//
//=============================================================================
#include "playerBullet.h"
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
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

#define P_BULLET_SPEED				(15.0f) // 弾の標準速度

#define P_BULLET_OFFSET				(40.0f) // 弾の発射始点（プレイヤーを基準にしてどれだけ離れるか？）
#define BULLET_SPEED_UP				(20.0f)	// 弾スピードアップアイテム取得時の速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/PLAYER/player_bullet_001.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static P_BULLET	pBullet[P_BULLET_MAX];	// プレイヤーバレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayerBullet(void)
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
	for (int i = 0; i < P_BULLET_MAX; i++)
	{
		pBullet[i].use = FALSE ;							// 生きているかどうか
		pBullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 初期値
		pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// rotation
		pBullet[i].w   = TEXTURE_WIDTH;						// 幅
		pBullet[i].h   = TEXTURE_HEIGHT;					// 高さ
		pBullet[i].texNo = 0;								// テクスチャ番号

		pBullet[i].upSpeed = BULLET_SPEED_UP;				// バレットスピードアップ

		pBullet[i].countAnim = 0;							// アニメカウンター(wait)
		pBullet[i].patternAnim = 0;							// アニメパターン番号

		pBullet[i].move = D3DXVECTOR3(0.0f, -P_BULLET_SPEED, 0.0f);		// 移動量

	}

	g_Load = TRUE;											// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayerBullet(void)
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
void UpdatePlayerBullet(void)
{
	for (int i = 0; i < P_BULLET_MAX; i++)
	{
		// 使っている弾だけ処理をする
		if (pBullet[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = pBullet[i].pos;

			//// アニメーション
			//pBullet[i].countAnim += 1.0f;

			//if (pBullet[i].countAnim > ANIM_WAIT)
			//{
			//	pBullet[i].countAnim = 0.0f;
			//	// パターンの切り替え(1ずつ大きくしておいてパターンの最大数になったらリセットする、これでループできる)
			//	pBullet[i].patternAnim = (pBullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			//}


			// 移動 
			{

				pBullet[i].pos += pBullet[i].move;


				// MAP外チェック
				BG *bg = GetBG();

				if (pBullet[i].pos.x + pBullet[i].w / 2 < 0.0f)
				{
					pBullet[i].use = FALSE;
				}

				if (pBullet[i].pos.x - pBullet[i].w / 2> bg->w)
				{
					pBullet[i].use = FALSE;
				}

				if (pBullet[i].pos.y + pBullet[i].h / 2 < 0.0f)
				{
					pBullet[i].use = FALSE;
				}

				if (pBullet[i].pos.y - pBullet[i].h / 2> bg->h)
				{
					pBullet[i].use = FALSE;
				}

				// 移動が終わったらプレイヤーとの当たり判定
				{
					ENEMY *enemy = GetEnemy();
					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (enemy->use == TRUE)
						{
							BOOL ans = CollisionBB(pBullet[i].pos, pBullet[i].w, pBullet[i].h,
								enemy[j].pos, enemy[i].w, enemy[j].h);

							// 当たっている？
							if (ans == TRUE)
							{
								DestroyEnemy(j);
								pBullet[i].use = FALSE;
							}
						}
					}
				}
			}

		}

//#ifdef _DEBUG
//		// デバッグ表示
//		PrintDebugProc("BulletNo%d  X:%f Y:%f\n", i, pBullet[i].pos.x, pBullet[i].pos.y);
//#endif

	}
	
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayerBullet(void)
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
	for (int i = 0; i < P_BULLET_MAX; i++)
	{
		if (pBullet[i].use == TRUE)		// この弾が使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[pBullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = pBullet[i].pos.x - bg->pos.x;	// 弾の表示位置X
			float py = pBullet[i].pos.y - bg->pos.y;	// 弾の表示位置Y
			float pw = pBullet[i].w;		// 弾の表示幅
			float ph = pBullet[i].h;		// 弾の表示高さ


			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (pBullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			
			float ty = (pBullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				pBullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// プレイヤーの弾をセット
//=============================================================================
void SetPlayerBullet(D3DXVECTOR3 pos, int type)
{

	for (int i = 0; i < P_BULLET_MAX; i++)
	{

		if (pBullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			pBullet[i].use = TRUE;			// 使用状態へ変更する
			pBullet[i].pos = pos;			// 座標をセット


			PLAYER *player = GetPlayer();
			// アイテムが発動している？
			if (player[0].bulletSpeedUpTime > 0.0f)
			{// Yes
				pBullet[i].upSpeed = P_BULLET_SPEED + BULLET_SPEED_UP;

			}
			else
			{
				pBullet[i].upSpeed = P_BULLET_SPEED;
			}


			switch (type)
			{
			case CHAR_DIR_DOWN:// 下方向
				pBullet[i].pos += D3DXVECTOR3(0.0f, P_BULLET_OFFSET, 0.0f);		// 前面から表示
				pBullet[i].move = D3DXVECTOR3(0.0f, pBullet[i].upSpeed, 0.0f);		// 移動量を初期化
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI);
				break;
			case CHAR_DIR_LEFT:// 左方向
				pBullet[i].pos += D3DXVECTOR3(-P_BULLET_OFFSET, 0.0f, 0.0f);	// 前面から表示
				pBullet[i].move = D3DXVECTOR3(-pBullet[i].upSpeed, 0.0f, 0.0f);		// 移動量を初期化
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, -D3DX_PI / 2);
				break;
			case CHAR_DIR_RIGHT:// 右方向
				pBullet[i].pos += D3DXVECTOR3(P_BULLET_OFFSET, 0.0f, 0.0f);		// 前面から表示
				pBullet[i].move = D3DXVECTOR3(pBullet[i].upSpeed, 0.0f, 0.0f);		// 移動量を初期化
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI / 2);
				break;
			case CHAR_DIR_UP:// 上方向
				pBullet[i].pos += D3DXVECTOR3(0.0f, -P_BULLET_OFFSET, 0.0f);	// 前面から表示
				pBullet[i].move = D3DXVECTOR3(0.0f, -pBullet[i].upSpeed, 0.0f);		// 移動量を初期化
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				break;
			default:
				break;
			}


			PlaySound(SOUND_LABEL_SE_shot000);
			return;							// 1発セットしたので終了する
		}
	}
}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
P_BULLET *GetPlayerBullet(void)
{

	return &pBullet[0];
}

