//=============================================================================
//

//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "player.h"
#include "collision.h"
#include "bullet.h"
#include "effect.h"
#include "score.h"
#include "sound.h"
#include "Vibration.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// エネミーサイズ
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(5)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X		(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y		(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM				(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define ENEMY_SPAWN_WAIT_DEFAULT		(200)	// エネミーのスポーン間隔
#define ENEMY_SPAWN_NUM				(3)		// １回のスポーンで現れる敵の数
#define ENEMY_SPAWN_RANGE			(300)	// エネミーのスポーン位置

#define ENEMY_CHANGE_MOVE_PROB		( 1)		// エネミーが移動方向を変える確率（%）

#define ENEMY_MOVE_SPD				(2.0f)	// エネミーの移動速度

#define ENEMY_ATK_RANGE				(500)	// エネミーの攻撃範囲	


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/ENEMY/enemy_001.png",
	"data/TEXTURE/ENEMY/enemy_002.png",
	"data/TEXTURE/ENEMY/enemy_003.png",
	"data/TEXTURE/ENEMY/enemy_004.png",
	"data/TEXTURE/ENEMY/enemy_005.png",
};


static BOOL		isLoad = FALSE;			// 初期化を行ったかのフラグ
static ENEMY		enemy[ENEMY_MAX];		// エネミー構造体

// 補間での移動テーブル
static INTERPOLATION_DATA moveTbl[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(600.0f, 400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(300.0f, 400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(300.0f, 100.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(600.0f, 100.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(600.0f, 400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },

};

static INTERPOLATION_DATA moveTbl2[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3( 800.0f, 800.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 3 },
	{ D3DXVECTOR3(1400.0f, 800.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ D3DXVECTOR3(1400.0f,1400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ D3DXVECTOR3( 800.0f,1400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ D3DXVECTOR3( 800.0f, 800.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },

};

static float spawnTimer = 0.0f;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
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


	// エネミー構造体の初期化
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		enemy[i].use = FALSE;
		enemy[i].pos = D3DXVECTOR3(50.0f + i*150, 100.0f, 0.0f);
		enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemy[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		enemy[i].w   = TEXTURE_WIDTH;
		enemy[i].h   = TEXTURE_HEIGHT;

		enemy[i].texNo = rand()% TEXTURE_MAX;

		enemy[i].countAnim = 0;
		enemy[i].patternAnim = 0;

		enemy[i].move = D3DXVECTOR3(ENEMY_MOVE_SPD, 0.0f, 0.0f);

		enemy[i].moveTime = 0.0f;	// 線形補間用のタイマーをクリア
		enemy[i].tblAdr = NULL;	// 再生するアニメデータの先頭アドレスをセット
		enemy[i].tblSize = 0;		// 再生するアニメデータのレコード数をセット

		enemy[i].countBullet = 0;
		enemy[i].countMax = 300;
	}

	// 線形補間で動かすエネミーにデータ登録
	enemy[0].use = TRUE;
	enemy[0].moveTime = 0.0f;		// 線形補間用のタイマーをクリア
	enemy[0].tblAdr = moveTbl;		// 再生するアニメデータの先頭アドレスをセット
	enemy[0].tblSize = sizeof(moveTbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	enemy[1].use = TRUE;
	enemy[1].moveTime = 0.0f;		// 線形補間用のタイマーをクリア
	enemy[1].tblAdr = moveTbl2;		// 再生するアニメデータの先頭アドレスをセット
	enemy[1].tblSize = sizeof(moveTbl2) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

	spawnTimer = 0.0f;

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	// エネミーのスポーン処理
	spawnTimer++;
	if (spawnTimer > ENEMY_SPAWN_WAIT_DEFAULT)
	{
		spawnTimer = 0.0f;
		
		int num = rand() % ENEMY_SPAWN_NUM;
		for (int i = 0; i < num; i++)
		{
			SetEnemy();
		}
	}

	PLAYER *player = GetPlayer();
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == TRUE)	// このエネミーが使われている？
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = enemy[i].pos;

			// アニメーション  
			enemy[i].countAnim++;
			if ((enemy[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				enemy[i].patternAnim = (enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			int diffX = (int)(player[0].pos.x - enemy[i].pos.x);
			int diffY = (int)(player[0].pos.y - enemy[i].pos.y);
			if (diffX + diffY < ENEMY_ATK_RANGE)
			{
				enemy[i].countBullet++;
			}
			
			if (enemy[i].countBullet >= enemy[i].countMax)
			{
				enemy[i].countBullet = 0;
				SetBullet(enemy[i].pos);

				// 線形補間以外のエネミーは移動方向を変える
				if (enemy[i].tblAdr == NULL)
				{
					float randomSpd = (float)(rand() % (int)(ENEMY_MOVE_SPD * 0.5f) + (int)(ENEMY_MOVE_SPD * 0.5f));
					float xSign = rand() % 3 - 1;
					float ySign = rand() % 3 - 1;
					enemy[i].move = D3DXVECTOR3(xSign* randomSpd, ySign * randomSpd, 0.0f);
				}
			}

			// 移動処理 
			if (enemy[i].tblAdr == moveTbl)	// 線形補間を実行する？
			{								// 線形補間の処理
				// 移動処理
				int		index = (int)enemy[i].moveTime;
				float	time = enemy[i].moveTime - index;
				int		size = enemy[i].tblSize;

				float dt = 1.0f / enemy[i].tblAdr[index].frame;	// 1フレームで進める時間
				enemy[i].moveTime += dt;							// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					enemy[i].moveTime = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				enemy[i].pos = (1 - time) * enemy[i].tblAdr[index].pos + time * enemy[i].tblAdr[index + 1].pos;

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				enemy[i].rot = (1 - time) * enemy[i].tblAdr[index].rot + time * enemy[i].tblAdr[index + 1].rot;

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				enemy[i].scl = (1 - time) * enemy[i].tblAdr[index].scl + time * enemy[i].tblAdr[index + 1].scl;

			}
			else if (enemy[i].tblAdr == moveTbl2) // 非線形での補間
			{

				// 移動処理
				int		index = (int)enemy[i].moveTime;
				float	time = enemy[i].moveTime - index;
				int		size = enemy[i].tblSize;

				if (index > 0)	// ゴールをオーバーしていたら、最初へ戻す
				{
					enemy[i].moveTime = 0.0f;
					index = 0;
				}

				float dt = 1.0f / enemy[i].tblAdr[index].frame;	// 1フレームで進める時間
				enemy[i].moveTime += dt;							// アニメーションの合計時間に足す

				// 非線形の補間(ベジエ曲線？)
				enemy[i].pos = (1 - time) * (1 - time) * (1 - time) * (1 - time) * enemy[i].tblAdr[index].pos
					+ 4 * (1 - time) * (1 - time) * (1 - time) * time * enemy[i].tblAdr[index + 1].pos
					+ 6 * (1 - time) * (1 - time) * time * time * enemy[i].tblAdr[index + 2].pos
					+ 4 * (1 - time) * time * time * time * enemy[i].tblAdr[index + 3].pos
					+ time * time * time * time * enemy[i].tblAdr[index + 4].pos;


			}
			else
			{
				enemy[i].pos += enemy[i].move;
			}

			// 向きの更新処理
			D3DXVECTOR3 diff = enemy[i].pos - pos_old;
			float radian = atan2f(diff.y, diff.x);
			enemy[i].rot.z = radian + D3DX_PI * 0.5f;

			// MAP外判定
			{
				BG *bg = GetBG();

				if ((enemy[i].pos.x < -enemy[i].w) || (enemy[i].pos.x > bg->w + enemy[i].w) ||
					(enemy[i].pos.y < -enemy[i].h) || (enemy[i].pos.y > bg->w + enemy[i].h))
				{
					enemy[i].use = FALSE;
				}
				
			}



			// プレイヤーとの当たり判定
			PLAYER *player = GetPlayer();
			// シールドが発動している？
			if (player[0].shieldTime <= 0.0f)
			{// No
				PLAYER *player = GetPlayer();
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(enemy[i].pos, enemy[i].w, enemy[i].h,
							player[j].pos, player[j].w, player[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 揺れを表示
							SetVibration();
							DestroyEnemy(i);
							player[0].hp -= DAMAGE;

							if (player[0].hp <= 0)
							{
 								player[0].use = FALSE;
								SetFade(FADE_OUT, MODE_RESULT);
							}

						}
					}
				}
			}
		}

#ifdef _DEBUG
		// デバッグ表示
		if (enemy[i].use == TRUE)
		{
			PrintDebugProc("Enemy No%d  X:%f Y:%f\n", i, enemy[i].pos.x, enemy[i].pos.y);
		}
#endif

	}
	

}


//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[enemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = enemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = enemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = enemy[i].w;					// エネミーの表示幅
			float ph = enemy[i].h;					// エネミーの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				enemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// エネミー構造体の先頭アドレスを取得
//=============================================================================
ENEMY *GetEnemy(void)
{

	return &enemy[0];
}

void SetEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false)
		{
			enemy[i].texNo = rand() % TEXTURE_MAX;

			enemy[i].countAnim = 0;
			enemy[i].patternAnim = 0;

			enemy[i].move = D3DXVECTOR3(rand() % 2 * ENEMY_MOVE_SPD, rand() % 2 * ENEMY_MOVE_SPD, 0.0f);

			PLAYER *player = GetPlayer();
			float randomX = (float)((rand() % (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE); // -300～300
			float randomY = (float)((rand() % (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE); // -300～300

			enemy[i].pos.x = player[i].pos.x + randomX;
			enemy[i].pos.y = player[i].pos.y + randomY;

			enemy[i].use = true;

			break;
		}
	}
}

void DestroyEnemy(int i)
{
	enemy[i].use = false;

	// エフェクト
	SetEffect(EFFECT_EXPLOSION, D3DXVECTOR3(enemy[i].pos.x, enemy[i].pos.y, 0.0f), 100, 100);

	// スコア加算
	AddScore(ENEMY_DESTORY_SCORE);

	// SE
	PlaySound(SOUND_LABEL_SE_bomb000);
}
