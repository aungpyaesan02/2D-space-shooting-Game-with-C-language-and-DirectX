//=============================================================================
//
//
//=============================================================================
#include "main.h"
#include "bg.h"
#include "cursor.h"
#include "mouseSkill.h"
#include "sprite.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(MOUSESKILL_DW)		// マウススキル用のサイズ（横）
#define TEXTURE_HEIGHT				(MOUSESKILL_DH)		// マウススキル用のサイズ（縦）
#define TEXTURE_MAX					(1)					// テクスチャの数
#define WAVESHOT_MOVE_ADDITION		(12.0f)				// マウススキルの加算用（WAVESHOT）
#define STUN_SCL_ADDITION			(5.0f)				// マウススキルの加算用（STUN）
#define STUN_SCL_SMOLL				(75.0f)				// マウススキルの加算用（STUN）
#define STUN_SCL_BIG				(300.0f)			// マウススキルの加算用（STUN）
#define HOMING_MOVE_ADDITION		(0.03f)				// マウススキルの加算用（HOMING）
#define VARTEX_ROT_MAX				(31.4f)				// マウススキルの加算用（VOLTEX）
#define VORTEX_ROT_ADDITION			(0.314f)			// マウススキルの加算用（VORTEX）
#define VORTEX_SCL_ADDITION			(2.0f)				// マウススキルの加算用（VORTEX）
#define CROSS_SCL_BIG				(300.0f)			// マウススキルの加算用（CROSS）
#define CROSS_SCL_ADDITION			(12.0f)				// マウススキルの加算用（CROSS）
#define CROSS_INCLINATION			(0.785f)			// マウススキル用の傾き（CROSS）
#define CROSS_SWITCH_WIDTH			(0)					// マウススキルのアニメーション切り替え用（CROSS）
#define CROSS_SWITCH_HEIGHT			(1)					// マウススキルのアニメーション切り替え用（CROSS）
#define ANIM_WAIT					(120)				// アニメーションの切り替わるWait値

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *texturName[TEXTURE_MAX] = {
	"data/TEXTURE/MOUSE/mouse_Skill.png",

};


static BOOL	isLoad = FALSE;		// 初期化を行ったかのフラグ
static MOUSESKILL	mouseSkill[MOUSESKILL_MAX];	// マウス判定用パッド構造体


// 初期化処理
//=============================================================================
HRESULT InitMouseSkill(void)
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
	for (int i = 0; MOUSESKILL_MAX > i; i++)
	{
		mouseSkill[i].use = FALSE;
		mouseSkill[i].w = TEXTURE_WIDTH;
		mouseSkill[i].h = TEXTURE_HEIGHT;
		mouseSkill[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		mouseSkill[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		mouseSkill[i].texNo = 0;
		mouseSkill[i].countAnim = 0;
		mouseSkill[i].skillFlag = SET_SKILL_MAX;		// 未使用のスキルフラグをセット
		mouseSkill[i].skillSwitch = CROSS_SWITCH_WIDTH;	// CROSSスキル用のフラグをセット

	}
	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMouseSkill(void)
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
void UpdateMouseSkill(void)
{
	// マウススキル発動用関数
	MouseSkillSelect();			// ← この関数の中に詳細記述。（スキルの追加もこの関数内で行う）
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMouseSkill(void)
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

	for (int i = 0; MOUSESKILL_MAX > i; i++)
	{
		if (mouseSkill[i].use == TRUE && mouseSkill[i].skillFlag != SET_SKILL_MAX)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[mouseSkill[i].texNo]);

			// マウススキルの位置やテクスチャー座標を反映
			float px = mouseSkill[i].pos.x - bg->pos.x;		// マウス判定用パッドの表示位置X
			float py = mouseSkill[i].pos.y - bg->pos.y;		// マウス判定用パッドの表示位置Y
			float pw = mouseSkill[i].w;		// マウス判定用パッドの表示幅
			float ph = mouseSkill[i].h;		// マウス判定用パッドの表示高さ

			float tw = 1.0f;			// テクスチャの幅
			float th = 1.0f;			// テクスチャの高さ
			float tx = 0.0f;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				mouseSkill[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// マウススキル構造体の先頭アドレスを取得
//=============================================================================
MOUSESKILL *GetMouseSkill(void)
{
	return &mouseSkill[0];
}

//=============================================================================
// マウススキルの描画設定
//=============================================================================
void SetMouseSkill(D3DXVECTOR3 pos, int skill)
{
	// 未使用のマウススキルが無かったら発動しない
	for (int i = 0; i < MOUSESKILL_MAX; i++)
	{
		if (mouseSkill[i].use == FALSE)		// 未使用状態のマウススキルを見つける
		{
			mouseSkill[i].use = TRUE;		// 使用状態へ変更する
			mouseSkill[i].skillFlag = skill;	// 発動スキルをセット
			mouseSkill[i].pos = pos;		// 座標をセット
			return;							// セットしたので終了する
		}
	}
}

//=============================================================================
// 発動スキルのセレクト
//=============================================================================
void MouseSkillSelect(void)
{
	ENEMY *enemy = GetEnemy();
	int mouseSkillCount = 0;	// 処理したマウススキルの数

	for (int i = 0; i < MOUSESKILL_MAX; i++)
	{
		if (mouseSkill[i].use == TRUE)	// マウススキルが使われている？
		{// Yes

			// アニメーション
			switch (mouseSkill[i].skillFlag)
			{

				// 右向きにウェーブを発生するスキル
			case SET_SKILL_RIGHTSHOT:

				// 時間で消失
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.x += WAVESHOT_MOVE_ADDITION;

				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;


				// 左向きにウェーブを発生するスキル
			case SET_SKILL_LEFTSHOT:

				// 時間で消失
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.x -= WAVESHOT_MOVE_ADDITION;

				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;

				// 上向きにウェーブを発生するスキル
			case SET_SKILL_UPSHOT:

				// 時間で消失
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.y -= WAVESHOT_MOVE_ADDITION;

				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;

				// 下向きにウェーブを発生するスキル
			case SET_SKILL_DOWNSHOT:

				// 時間で消失
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.y += WAVESHOT_MOVE_ADDITION;

				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;

				// クロスして拡大するスキル
			case SET_SKILL_CROSS:

				// 時間で消失
				mouseSkill[i].countAnim++;

				mouseSkill[i].rot.z = CROSS_INCLINATION;
				{
					switch (mouseSkill[i].skillSwitch)
					{
					case CROSS_SWITCH_WIDTH:
						// 横方向に拡大
						mouseSkill[i].w += CROSS_SCL_ADDITION;
						if (mouseSkill[i].w > CROSS_SCL_BIG)
						{
							mouseSkill[i].w = TEXTURE_WIDTH;
							mouseSkill[i].skillSwitch = CROSS_SWITCH_HEIGHT;
						}
						break;

					case CROSS_SWITCH_HEIGHT:
						// 縦方向に拡大
						mouseSkill[i].h += CROSS_SCL_ADDITION;
						if (mouseSkill[i].h > CROSS_SCL_BIG)
						{
							mouseSkill[i].h = TEXTURE_HEIGHT;
							mouseSkill[i].skillSwitch = CROSS_SWITCH_WIDTH;
						}

						// SE
						PlaySound(SOUND_LABEL_SE_shot000);

						break;

					default:
						break;
					}
				}
				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].skillSwitch = CROSS_SWITCH_WIDTH;
					mouseSkill[i].rot.z = 0.0f;
					mouseSkill[i].w = TEXTURE_WIDTH;
					mouseSkill[i].h = TEXTURE_HEIGHT;
					mouseSkill[i].use = FALSE;
				}
				break;

				// 当たった敵をスタン状態にするスキル
			case SET_SKILL_STUN:

				// 時間で消失
				mouseSkill[i].countAnim++;

				// 横方向に縮小
				mouseSkill[i].w -= STUN_SCL_ADDITION;
				if (mouseSkill[i].w < STUN_SCL_SMOLL)
				{
					mouseSkill[i].w = TEXTURE_WIDTH;
				}
				// 縦方向に拡大
				mouseSkill[i].h += STUN_SCL_ADDITION;
				if (mouseSkill[i].h > STUN_SCL_BIG)
				{
					mouseSkill[i].h = TEXTURE_WIDTH;
				}

				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].w = TEXTURE_WIDTH;
					mouseSkill[i].h = TEXTURE_HEIGHT;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_mouseswip);

				break;

				// 敵をホーミングするスキル
			case SET_SKILL_HOMING:

				// 時間で消失
				mouseSkill[i].countAnim++;

				//{ // ホーミング処理 (線形補完の影響か微妙、こちらは不調とりあえず先頭をホーミング)
				//	for (int k = 0; ENEMY_MAX > k; k++)
				//	{
				//		D3DXVECTOR3 pos = enemy[k].pos - mouseSkill[i].pos;
				//		mouseSkill[i].pos += pos * HOMING_MOVE_ADDITION;
				//	}
				//}

				{ // ホーミング処理 (単体)
					D3DXVECTOR3 pos = enemy->pos - mouseSkill[i].pos;
					mouseSkill[i].pos += pos * HOMING_MOVE_ADDITION;
				}

				// 一定の時間を超えたら未使用にする
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_door);

				break;

				// 敵を吸い込むスキル
			case SET_SKILL_VORTEX:

				// 拡大して表示する
				mouseSkill[i].w = TEXTURE_WIDTH * VORTEX_SCL_ADDITION;
				mouseSkill[i].h = TEXTURE_WIDTH * VORTEX_SCL_ADDITION;

				// 一定数の回転で消失
				mouseSkill[i].rot.z += VORTEX_ROT_ADDITION;

				// 現段階では吸い込めず。。。

				// 一定の回転数を超えたら未使用にする
				if (mouseSkill[i].rot.z > VARTEX_ROT_MAX)
				{
					mouseSkill[i].rot.z = 0.0f;
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].w = TEXTURE_WIDTH;
					mouseSkill[i].h = TEXTURE_HEIGHT;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_recharge);

				break;


				/* スキルが追加されたらここに追加していく */


			default:
				break;
			}


			// ENEMYとの当たり判定
			{
				for (int j = 0; ENEMY_MAX > j; j++)
				{

					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(mouseSkill[i].pos, mouseSkill[i].w, mouseSkill[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);

						switch (ans)
						{
						case TRUE:
						{
							switch (mouseSkill[i].skillFlag)
							{
							case SET_SKILL_RIGHTSHOT:
								// 当たった時の処理
								DestroyEnemy(j);
								
								break;

							case SET_SKILL_LEFTSHOT:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							case SET_SKILL_UPSHOT:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							case SET_SKILL_DOWNSHOT:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							case SET_SKILL_CROSS:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							case SET_SKILL_STUN:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							case SET_SKILL_HOMING:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							case SET_SKILL_VORTEX:
								// 当たった時の処理
								DestroyEnemy(j);
								break;

							default:
								break;
							}
						}
						break;

						default:
							break;
						}
					}
				}
			}
		mouseSkillCount++;	// マウススキルの加算
		}
	}

}