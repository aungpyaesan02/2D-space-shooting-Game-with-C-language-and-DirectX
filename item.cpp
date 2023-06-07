//=============================================================================
//
//
//=============================================================================
#include "item.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "player.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(35)			// アイテムサイズ
#define TEXTURE_HEIGHT				(35)			// 
#define TEXTURE_MAX					(E_ITEM_MAX)	// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)				// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)				// アニメーションの切り替わるWait値

#define SHIELD_TIME					(5.0f * 6.0f)	// シールド有効時間	（x秒 ＝ (xf * 6.0f)）
#define HP_RECOVERY					(1)				// 回復量
#define MOVESPEED_UP_TIME			(3.0f * 6.0f)	// 移動加速時間		（x秒 ＝ (xf * 6.0f)）
#define BULLETSPEED_UP_TIME			(10.0f * 6.0f)	// 弾加速時間		（x秒 ＝ (xf * 6.0f)）


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *texturName[TEXTURE_MAX] = {
	"data/TEXTURE/ITEM/hp001.png",
	"data/TEXTURE/ITEM/shield.png",
	"data/TEXTURE/ITEM/speed_up.png",
	"data/TEXTURE/ITEM/bullet_speed_up.png",
};


static BOOL		isLoad = FALSE;			// 初期化を行ったかのフラグ
static ITEM		item[ITEM_MAX];			// アイテム構造体

static float itemPopTime;						// アイテムポップ時間


// 補間での移動テーブル
static INTERPOLATION_DATA moveTbl0[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(700.0f, 700.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(750.0f, 700.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(700.0f, 700.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};
static INTERPOLATION_DATA moveTbl1[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(300.0f, 300.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(150.0f, 300.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(300.0f, 300.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};
static INTERPOLATION_DATA moveTbl2[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(1600.0f, 1200.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1300.0f, 1200.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1600.0f, 1200.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};
static INTERPOLATION_DATA moveTbl3[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(1500.0f, 500.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1200.0f, 500.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1500.0f, 500.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
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


	// アイテム構造体の初期化
	for (int i = 0; i < ITEM_MAX; i++)
	{
		item[i].display = FALSE;
		item[i].pos     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		item[i].rot     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		item[i].scl     = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		item[i].w       = TEXTURE_WIDTH;
		item[i].h       = TEXTURE_HEIGHT;
		item[i].move    = D3DXVECTOR3(4.0f, 0.0f, 0.0f);

		item[i].texNo = i;

		item[i].countAnim = 0;
		item[i].patternAnim = 0;

		item[i].type = 0;

		item[i].moveTime = 0.0f;	// 線形補間用のタイマーをクリア
		item[i].tblAdr   = NULL;	// 再生するアニメデータの先頭アドレスをセット
		item[i].tblSize  = 0;		// 再生するアニメデータのレコード数をセット

	}



	/*アイテムに線形補間のデータを登録*/
	{
		// 0番目のアイテムにmoveTbl0を登録
		item[E_ITEM_HP].moveTime = 0.0f;			// 線形補間用のタイマーをクリア
		item[E_ITEM_HP].tblAdr   = moveTbl0;		// 再生するアニメデータの先頭アドレスをセット
		item[E_ITEM_HP].tblSize  = sizeof(moveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		// 1番目のアイテムにmoveTbl1を登録
		item[E_ITEM_SHIELD].moveTime = 0.0f;			// 線形補間用のタイマーをクリア
		item[E_ITEM_SHIELD].tblAdr = moveTbl1;		// 再生するアニメデータの先頭アドレスをセット
		item[E_ITEM_SHIELD].tblSize = sizeof(moveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		// 2番目のアイテムにmoveTbl2を登録
		item[E_ITEM_MOVESPEED_UP].moveTime = 0.0f;			// 線形補間用のタイマーをクリア
		item[E_ITEM_MOVESPEED_UP].tblAdr = moveTbl2;		// 再生するアニメデータの先頭アドレスをセット
		item[E_ITEM_MOVESPEED_UP].tblSize = sizeof(moveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		// 3番目のアイテムにmoveTbl3を登録
		item[E_ITEM_BULLETSPEED_UP].moveTime = 0.0f;			// 線形補間用のタイマーをクリア
		item[E_ITEM_BULLETSPEED_UP].tblAdr = moveTbl3;		// 再生するアニメデータの先頭アドレスをセット
		item[E_ITEM_BULLETSPEED_UP].tblSize = sizeof(moveTbl3) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	}




	// アイテムポップ時間
	itemPopTime = 0;


	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitItem(void)
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
void UpdateItem(void)
{
	// 時間計測
	itemPopTime += 1.0f;
	float minutes = itemPopTime / 3600.0f;	// 1/3600フレームへ変換

	// 特定の時間にランダムでアイテムを出力
	if (minutes >= 0.2f)
	{
		for (int i = 0; i < TEXTURE_MAX; i++)
		{
			//アイテムがすでに存在しているかチェックする
			if (item[i].display == TRUE)
				continue;


			int itemPop = rand() % TEXTURE_MAX;
			BOOL checkPop = FALSE;			// ポップしたか確認

			// 存在していない？
			if (item[itemPop].display == FALSE)
			{// Yes
				item[itemPop].display = TRUE;
				checkPop = TRUE;
			}

			// アイテムポップしたら時間を初期化
			if (checkPop == TRUE)
			{
				itemPopTime = 0.0f;
				minutes = 0.0f;
				checkPop = FALSE;
			}
		}
	}


	for (int i = 0; i < ITEM_MAX; i++)
	{

		// このアイテムは未使用かつ表示中？
		if (item[i].display == TRUE)
		{// Yes
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 posOld = item[i].pos;

			// アニメーション  
			item[i].countAnim++;
			if ((item[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				item[i].patternAnim = (item[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// 移動処理 
			if (item[i].tblAdr != NULL)	// 線形補間を実行する？
			{								// 線形補間の処理
				// 移動処理
				int		index = (int)item[i].moveTime;
				float	time = item[i].moveTime - index;
				int		size = item[i].tblSize;

				float dt = 1.0f / item[i].tblAdr[index].frame;	// 1フレームで進める時間
				item[i].moveTime += dt;							// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					item[i].moveTime = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				item[i].pos = (1 - time) * item[i].tblAdr[index].pos + time * item[i].tblAdr[index + 1].pos;

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				item[i].rot = (1 - time) * item[i].tblAdr[index].rot + time * item[i].tblAdr[index + 1].rot;

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				item[i].scl = (1 - time) * item[i].tblAdr[index].scl + time * item[i].tblAdr[index + 1].scl;

			}
			else
			{
				item[i].pos += item[i].move;
			}

			// MAP外判定
			{
				BG *bg = GetBG();

				if ((item[i].pos.x < -item[i].w) || (item[i].pos.x > bg->w + item[i].w) ||
					(item[i].pos.y < -item[i].h) || (item[i].pos.y > bg->w + item[i].h))
				{
					item[i].display = FALSE;
				}

			}






			// プレイヤー情報を取得
			PLAYER *player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				//敵の有効フラグをチェックする
				if (player[j].use == FALSE)
					continue;

				BOOL ans = CollisionBB(player[j].pos, player[j].w, player[j].h,
					item[i].pos, item[i].w, item[i].h);
				if (ans == TRUE)
				{
					switch (i)
					{
					case E_ITEM_HP:
						SetPlayerHP(HP_RECOVERY);
						break;
					case E_ITEM_SHIELD:
						SetPlayerShield(SHIELD_TIME);
						break;
					case E_ITEM_MOVESPEED_UP:
						SetPlayerMoveSpeed(MOVESPEED_UP_TIME);
						break;
					case E_ITEM_BULLETSPEED_UP:
						SetPlayerBulletSpeed(BULLETSPEED_UP_TIME);
						break;
					}

					item[i].display = FALSE;

				}
			}

		}

#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Item No%d  X:%f Y:%f\n", i, item[i].pos.x, item[i].pos.y);
	}
	PrintDebugProc("seconds = %f\n", itemPopTime);
	PrintDebugProc("minutes = %f\n", minutes);

	PrintDebugProc("item0 = %d\n", item[E_ITEM_HP].display);
	PrintDebugProc("item1 = %d\n", item[E_ITEM_SHIELD].display);
	PrintDebugProc("item2 = %d\n", item[E_ITEM_MOVESPEED_UP].display);
	PrintDebugProc("item3 = %d\n", item[E_ITEM_BULLETSPEED_UP].display);
#endif
	}
}
	




//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
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

	for (int i = 0; i < ITEM_MAX; i++)
	{
		// このアイテムは未使用かつ表示中？
		if (item[i].display == TRUE)
		{// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[item[i].texNo]);

			//アイテムの位置やテクスチャー座標を反映
			float px = item[i].pos.x - bg->pos.x;	// アイテムの表示位置X
			float py = item[i].pos.y - bg->pos.y;	// アイテムの表示位置Y
			float pw = item[i].w;					// アイテムの表示幅
			float ph = item[i].h;					// アイテムの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(item[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(item[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				item[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}
}



//=============================================================================
// エネミー構造体の先頭アドレスを取得
//=============================================================================
ITEM *GetItem(void)
{

	return &item[0];
}




