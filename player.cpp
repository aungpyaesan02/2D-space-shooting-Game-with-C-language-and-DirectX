//=============================================================================
//
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "enemy.h"
#include "bullet.h"
#include "playerBullet.h"
#include "collision.h"
#include "sound.h"
#include "fade.h"
#include "result.h"
#include "file.h"
#include "vibration.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(80)	// キャラサイズ
#define TEXTURE_HEIGHT				(80)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define PLAYER_SPEED				(7.0f)	// プレイヤーの標準速度

#define HP_MAX						(10)	// HPの最大値
#define SPEED_UP					(4.0f)		// スピードアップアイテム取得時の速度



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/PLAYER/ship_001.png",
};


static BOOL		isLoad = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	player[PLAYER_MAX];	// プレイヤー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			TexturName[i],
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i].use = TRUE;
		player[i].pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);	// 中心点から表示
		player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		player[i].w   = TEXTURE_WIDTH;
		player[i].h   = TEXTURE_HEIGHT;
		player[i].texNo = 0;
		player[i].hp = HP_MAX;
		player[i].shieldTime = 0.0f;
		player[i].speedUpTime = 0.0f;
		player[i].bulletSpeedUpTime = 0.0f;

		player[i].hp = HP_MAX;

		player[i].countAnim = 0;
		player[i].patternAnim = 0;

		player[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// 移動量

		player[i].dir = CHAR_DIR_DOWN;						// 下向き
		player[i].moving = FALSE;							// Key押されているかフラグ
		player[i].patternAnim = player[i].dir * TEXTURE_PATTERN_DIVIDE_X;
	}

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (player[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 posOld = player[i].pos;


			// アニメーション 
			if (player[i].moving == TRUE)
			{
				player[i].countAnim += 1.0f;
				if (player[i].countAnim > ANIM_WAIT)
				{
					player[i].countAnim = 0.0f;
					// パターンの切り替え
					player[i].patternAnim = (player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}

			// キー入力で移動 
			{
				// 移動速度
				if (player[0].speedUpTime > 0.0f)
				{
					player[i].move.x = PLAYER_SPEED + SPEED_UP;

				}
				else
				{
					player[i].move.x = PLAYER_SPEED;
				}

				float speed = player[i].move.x;

				player[i].moving = FALSE;


				if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
				{
					player[i].pos.y += speed;
					player[i].dir = CHAR_DIR_DOWN;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 3.14f);
					player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
				{
					player[i].pos.y -= speed;
					player[i].dir = CHAR_DIR_UP;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					player[i].moving = TRUE;
				}

				if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
				{
					player[i].pos.x += speed;
					player[i].dir = CHAR_DIR_RIGHT;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 1.57f);
					player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
				{
					player[i].pos.x -= speed;
					player[i].dir = CHAR_DIR_LEFT;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, -1.57f);
					player[i].moving = TRUE;
				}

				// ゲームパッドでで移動処理
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					player[i].pos.y += speed;
					player[i].dir = CHAR_DIR_DOWN;
					player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_UP))
				{
					player[i].pos.y -= speed;
					player[i].dir = CHAR_DIR_UP;
					player[i].moving = TRUE;
				}

				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					player[i].pos.x += speed;
					player[i].dir = CHAR_DIR_RIGHT;
					player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					player[i].pos.x -= speed;
					player[i].dir = CHAR_DIR_LEFT;
					player[i].moving = TRUE;
				}

				// 弾発射
				//if (GetKeyboardPress(DIK_SPACE) || IsButtonPressed(0, BUTTON_R))
				//{

				// プレイヤーの向きでバレットの方向を変える
				switch (player[i].dir)
				{
				case CHAR_DIR_DOWN:// 下方向
					SetPlayerBullet(player[i].pos, CHAR_DIR_DOWN);
					break;
				case CHAR_DIR_LEFT:// 左方向
					SetPlayerBullet(player[i].pos, CHAR_DIR_LEFT);
					break;
				case CHAR_DIR_RIGHT:// 右方向
					SetPlayerBullet(player[i].pos, CHAR_DIR_RIGHT);
					break;
				case CHAR_DIR_UP:	// 上方向
					SetPlayerBullet(player[i].pos, CHAR_DIR_UP);
					break;
				default:
					break;
				}

				//}


				// MAP外チェック
				BG *bg = GetBG();

				if (player[i].pos.x - TEXTURE_WIDTH / 2 < 0.0f)
				{
					player[i].pos.x = 0.0f + TEXTURE_WIDTH / 2;
				}

				if (player[i].pos.x + TEXTURE_WIDTH / 2 > bg->w)
				{
					player[i].pos.x = bg->w - TEXTURE_WIDTH / 2;
				}

				if (player[i].pos.y - TEXTURE_HEIGHT / 2 < 0.0f)
				{
					player[i].pos.y = 0.0f + TEXTURE_HEIGHT / 2;
				}

				if (player[i].pos.y + TEXTURE_HEIGHT / 2 > bg->h)
				{
					player[i].pos.y = bg->h - TEXTURE_HEIGHT / 2;
				}

				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// 移動が終わったらエネミーとの当たり判定
				{
					ENEMY *enemy = GetEnemy();
					// シールドが発動している？
					if (player[0].shieldTime <= 0.0f)
					{// No
						// エネミーの数分当たり判定を行う
						for (int j = 0; j < ENEMY_MAX; j++)
						{
							// 生きてるエネミーと当たり判定をする
							if (enemy[j].use == TRUE)
							{

								BOOL ans = CollisionBB(player[i].pos, player[i].w, player[i].h,
									enemy[j].pos, enemy[j].w, enemy[j].h);

								// 当たっている？
								if (ans == TRUE)
								{
									// 当たった時の処理
									player[0].hp -= DAMAGE;
									SetVibration();
									DestroyEnemy(j);
									if (player[0].hp <= 0)
									{
										player[i].use = FALSE;
										SetFade(FADE_OUT, MODE_RESULT);
									}

								}
							}
						}
					}
				}
			}
		}


		// シールド有効時間の計算
		player[0].shieldTime -= 0.1f;
		if (player[0].shieldTime < 0.0f)
		{
			player[0].shieldTime = 0.0f;
		}		

		// プレイヤーのスピードアップ時間の計算
		player[0].speedUpTime -= 0.1f;
		if (player[0].speedUpTime < 0.0f)
		{
			player[0].speedUpTime = 0.0f;
		}

		// 弾のスピードアップ時間の計算
		player[0].bulletSpeedUpTime -= 0.1f;
		if (player[0].bulletSpeedUpTime < 0.0f)
		{
			player[0].bulletSpeedUpTime = 0.0f;
		}


#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Player No%d  X:%f Y:%f\n", i, player[i].pos.x, player[i].pos.y);
		PrintDebugProc("hp = %d\n", player[i].hp);
		PrintDebugProc("ShieldTime = %f\n", player[0].shieldTime);
		PrintDebugProc("SpeedUpTime = %f\n", player[0].speedUpTime);

		// フェード用のテスト
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter押したら、ステージを切り替える
			// リザルトにスコアをセットする
			SetResult(GetScore());
			SaveHighscore();
			SetFade(FADE_OUT, MODE_RESULT);	// リザルト画面へ

			SetFade(FADE_OUT, MODE_RESULT);
		}


#endif

	}
	
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = player[i].w;		// プレイヤーの表示幅
			float ph = player[i].h;		// プレイヤーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// プレイヤー構造体の先頭アドレスを取得
//=============================================================================
PLAYER *GetPlayer(void)
{

	return &player[0];
}

//=============================================================================
// プレイヤー回復の関数
//=============================================================================
void SetPlayerHP(int hp)
{
	player[0].hp += hp;
	if (player[0].hp > 10)
	{
		player[0].hp = 10;
	}
}

//=============================================================================
// プレイヤーシールド発動時間を設定する関数
//=============================================================================
void SetPlayerShield(float shieldTime)
{
	player[0].shieldTime = shieldTime;
}

//=============================================================================
// プレイヤー移動速度がアップする時間を設定する関数
//=============================================================================
void SetPlayerMoveSpeed(float moveSpeedUp)
{
	player[0].speedUpTime = moveSpeedUp;
}

//=============================================================================
// プレイヤーの弾速度がアップする時間を設定する関数
//=============================================================================
void SetPlayerBulletSpeed(float bulletSpeedUp)
{
	player[0].bulletSpeedUpTime = bulletSpeedUp;
}