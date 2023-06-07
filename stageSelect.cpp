//=============================================================================
//

//
//=============================================================================
#include "stageSelect.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(9)				// テクスチャの数

#define STAGESELECT_BG_WIDTH		(SCREEN_WIDTH)	// ステージセレクト背景サイズ（縦）
#define STAGESELECT_BG_HEIGHT		(SCREEN_HEIGHT)	// ステージセレクト背景サイズ（横）
#define STAGESELECT_FLAME_WIDTH		(128)			// ステージセレクト背景サイズ（縦）
#define STAGESELECT_FLAME_HEIGHT	(128)			// ステージセレクト背景サイズ（横）
#define ADJUSUMENT_WIDTH			(STAGESELECT_FLAME_WIDTH / 2)	// 背景調整用（縦）
#define ADJUSUMENT_HEIGHT			(STAGESELECT_FLAME_HEIGHT / 2)	// 背景調整用（横）
#define COMINGSOON_WIDTH			(540)			// 準備画面（縦）
#define COMINGSOON_HEIGHT			(230)			// 準備画面（横）

#define CHOICE_MAX					(7)				// 選択肢のMax数

#define ANIM_PATTERN_DIVIDE_X		(5)				// セレクトアニメーションのテクスチャ内分割数（X)
#define ANIM_PATTERN_DIVIDE_Y		(5)				// セレクトアニメーションのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(ANIM_PATTERN_DIVIDE_X*ANIM_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(12)			// セレクトアニメーションの切り替わるWait値

#define DOOR_PATTERN_DIVIDE_X		(5)				// 開くドアアニメーションのテクスチャ内分割数（X)
#define DOOR_PATTERN_DIVIDE_Y		(1)				// 開くドアアニメーションのテクスチャ内分割数（Y)
#define DOOR_ANIM_PATTERN_NUM		(DOOR_PATTERN_DIVIDE_X*DOOR_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define DOOR_ANIM_WAIT				(12)			// ドアアニメーションの切り替わるWait値

#define COMINGSOON_PATTERN_DIVIDE_X	(3)				// 開くドアアニメーションのテクスチャ内分割数（X)
#define COMINGSOON_PATTERN_DIVIDE_Y	(1)				// 開くドアアニメーションのテクスチャ内分割数（Y)
#define COMINGSOON_ANIM_PATTERN_NUM	(COMINGSOON_PATTERN_DIVIDE_X*COMINGSOON_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define COMINGSOON_ANIM_WAIT		(16)			// ドアアニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/STAGE_SELECT/select_Stage.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_BG.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame_Status.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame_Base.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame_Title.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Door.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Anim.png",
	"data/TEXTURE/STAGE_SELECT/comingSoon.png",

};


static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ
static STAGESELECT	stageSelect[STAGESELECT_MAX];
static STAGESELECT_FLAME	stageSelectFlame[CHOICE_MAX];
static STAGESELECT_DOOR		stageSelectDoor[DOOR_MAX];			// 開くドアアニメーション用構造体
static STAGESELECT_ANIM		stageSelectAnim[ANIM_MAX];			// ステージアニメーション用構造体
static COMINGSOON			comingSoon;			// ステージアニメーション用構造体

static D3DXVECTOR3 selectFlame[CHOICE_MAX] =
{
	D3DXVECTOR3(120.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(240.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(360.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(SCREEN_CENTER_X - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(600.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(720.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(840.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStageSelect(void)
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


	// ステージセレクト背景等の初期化
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		stageSelect[i].w = STAGESELECT_BG_WIDTH;
		stageSelect[i].h = STAGESELECT_BG_HEIGHT;
		stageSelect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	stageSelect[0].texNo = 1;

	stageSelect[1].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[1].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[1].pos = D3DXVECTOR3(120.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[1].texNo = 2;

	stageSelect[2].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[2].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[2].pos = D3DXVECTOR3(240.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[2].texNo = 3;

	stageSelect[3].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[3].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[3].pos = D3DXVECTOR3(360.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[3].texNo = 3;

	stageSelect[4].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[4].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[4].pos = D3DXVECTOR3(SCREEN_CENTER_X - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[4].texNo = 3;

	stageSelect[5].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[5].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[5].pos = D3DXVECTOR3(600.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[5].texNo = 3;

	stageSelect[6].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[6].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[6].pos = D3DXVECTOR3(720.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[6].texNo = 3;

	stageSelect[7].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[7].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[7].pos = D3DXVECTOR3(840.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[7].texNo = 4;

	stageSelect[8].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[8].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[8].pos = stageSelect[1].pos;
	stageSelect[8].texNo = 5;

	stageSelectFlame[0].flameSw = 0;	//枠座標の切り替え用


	// セレクトアニメーションの初期化
	for (int i = 0; i < ANIM_MAX; i++)
	{
		stageSelectAnim[i].flameFlag = 'N';
		stageSelectAnim[i].w = STAGESELECT_FLAME_WIDTH;
		stageSelectAnim[i].h = STAGESELECT_FLAME_HEIGHT;
		stageSelectAnim[i].texNo = 7;
		stageSelectAnim[i].countAnim = 0;
		stageSelectAnim[i].patternAnim = 0;
		stageSelectAnim[i].selectNo = CHAR_SELECT_STAGE_B;
		stageSelectAnim[i].patternAnim = stageSelectAnim[i].selectNo * ANIM_PATTERN_DIVIDE_X;

	}

	stageSelectAnim[0].pos = D3DXVECTOR3(240.0f, SCREEN_CENTER_Y, 0.0f);
	stageSelectAnim[1].pos = D3DXVECTOR3(360.0f, 405.0f, 0.0f);
	stageSelectAnim[2].pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	stageSelectAnim[3].pos = D3DXVECTOR3(600.0f, 405.0f, 0.0f);
	stageSelectAnim[4].pos = D3DXVECTOR3(720.0f, SCREEN_CENTER_Y, 0.0f);

	// 開くドアアニメーションの初期化
	for (int i = 0; i < DOOR_MAX; i++)
	{
		stageSelectDoor[i].flameFlag = 'N';
		stageSelectDoor[i].w = STAGESELECT_FLAME_WIDTH;
		stageSelectDoor[i].h = STAGESELECT_FLAME_HEIGHT;
		stageSelectDoor[i].texNo = 6;
		stageSelectDoor[i].countAnim = 0;
		stageSelectDoor[i].patternAnim = 0;
	}

	stageSelectDoor[0].pos = D3DXVECTOR3(240.0f, SCREEN_CENTER_Y, 0.0f);
	stageSelectDoor[1].pos = D3DXVECTOR3(360.0f, 405.0f, 0.0f);
	stageSelectDoor[2].pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	stageSelectDoor[3].pos = D3DXVECTOR3(600.0f, 405.0f, 0.0f);
	stageSelectDoor[4].pos = D3DXVECTOR3(720.0f, SCREEN_CENTER_Y, 0.0f);

	// 準備画面の初期化
	comingSoon.useFlag = 'N';
	comingSoon.w = COMINGSOON_WIDTH;
	comingSoon.h = COMINGSOON_HEIGHT;
	comingSoon.pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	comingSoon.texNo = 8;
	comingSoon.countAnim = 0;
	comingSoon.patternAnim = 0;

	isLoad = TRUE;						// データの初期化を行った
	return S_OK;

}


//=============================================================================
// 終了処理
//=============================================================================
void UninitStageSelect(void)
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
void UpdateStageSelect(void)
{
	// ドアアニメーション
	{

		for (int i = 0; i < DOOR_MAX; i++)
		{
			// フラグによってアニメーションの有無を決める
			switch (stageSelectDoor[i].flameFlag)
			{
			case 'Y':
				stageSelectDoor[i].countAnim += 1.0f;
				if (stageSelectDoor[i].countAnim > DOOR_ANIM_WAIT)
				{
					stageSelectDoor[i].countAnim = 0.0f;
					// アニメパターンの切り替え
					stageSelectDoor[i].patternAnim++;
				}
				break;

			case 'N':
				break;

			default:
				break;
			}
		}

	}

	// セレクトアニメーション
	{

		for (int i = 0; i < ANIM_MAX; i++)
		{
			// フラグによってアニメーションの有無を決める
			switch (stageSelectAnim[i].flameFlag)
			{
			case 'Y':
				stageSelectAnim[i].countAnim += 1.0f;
				if (stageSelectAnim[i].countAnim > ANIM_WAIT)
				{
					stageSelectAnim[i].countAnim = 0.0f;
					// パターンの切り替え
					stageSelectAnim[i].patternAnim = (stageSelectAnim[i].selectNo * ANIM_PATTERN_DIVIDE_X) + ((stageSelectAnim[i].patternAnim + 1) % ANIM_PATTERN_DIVIDE_X);
				}
				break;
			case 'N':
				break;

			default:
				break;
			}
		}

	}

	// 準備画面アニメーション
	{
		switch (comingSoon.useFlag)
		{
		case 'Y':
			comingSoon.countAnim += 1.0f;
			if (comingSoon.countAnim > COMINGSOON_ANIM_WAIT)
			{
				comingSoon.countAnim = 0.0f;
				// アニメパターンの切り替え
				comingSoon.patternAnim++;
				if (comingSoon.patternAnim >= COMINGSOON_ANIM_PATTERN_NUM)
				{
					comingSoon.patternAnim = 0;
					comingSoon.useFlag = 'N';
				}
			}
			break;
		case 'N':
			break;
		default:
			break;
		}

	}

	// 移動した位置でアニメーションを発生させるかを決める（デフォルトは'N'を入れる）
	{
		for (int i = 0; i < ANIM_MAX; i++)
		{
			stageSelectDoor[i].flameFlag = 'N';
			stageSelectAnim[i].flameFlag = 'N';
		}
	}

	// キーボードで入力処理(枠の移動)
	{
		if (GetKeyboardTrigger(DIK_RIGHT))
		{
			stageSelectFlame[0].flameSw++;	// 右に選択を進める
			if (stageSelectFlame[0].flameSw > CHOICE_MAX - 1) stageSelectFlame[0].flameSw = CHOICE_MAX - 1;		// 要素数Max -1（6）以上になったら最後に戻す
			PlaySound(SOUND_LABEL_SE_button);
		}
		else if (GetKeyboardTrigger(DIK_LEFT))
		{
			stageSelectFlame[0].flameSw--;	// 左に選択を進める
			if (stageSelectFlame[0].flameSw < 0) stageSelectFlame[0].flameSw = 0;	//マイナスになったら0に戻す
			PlaySound(SOUND_LABEL_SE_button);
		}

		stageSelect[8].pos = selectFlame[stageSelectFlame[0].flameSw];	//枠の座標
	}

	// フェード先の決定とアニメーションに入れるフラグの振り分け
	{
		if (stageSelect[1].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// STATUSの位置にある時にEnter押したら、ステータス選択画面に遷移（今は準備顔面を表示）
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}

		else if (stageSelect[2].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[0].flameFlag = 'Y';
			stageSelectAnim[0].flameFlag = 'Y';
			stageSelectAnim[0].selectNo = CHAR_SELECT_STAGE_1;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// ステージ１の位置にある時にEnter押したら、ゲームへ遷移
				SetFade(FADE_OUT, MODE_GAME);
				PlaySound(SOUND_LABEL_SE_defend001);
			}
		}
		else if (stageSelect[3].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[1].flameFlag = 'Y';
			stageSelectAnim[1].flameFlag = 'Y';
			stageSelectAnim[1].selectNo = CHAR_SELECT_STAGE_2;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// ステージ２の位置にある時にEnter押したら、ゲームへ遷移（今は準備画面を表示）
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[4].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[2].flameFlag = 'Y';
			stageSelectAnim[2].flameFlag = 'Y';
			stageSelectAnim[2].selectNo = CHAR_SELECT_STAGE_3;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// ステージ３の位置にある時にEnter押したら、ゲームへ遷移（今は準備画面を表示）
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[5].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[3].flameFlag = 'Y';
			stageSelectAnim[3].flameFlag = 'Y';
			stageSelectAnim[3].selectNo = CHAR_SELECT_STAGE_4;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// ステージ４の位置にある時にEnter押したら、ゲームへ遷移（今は準備画面を表示）
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[6].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[4].flameFlag = 'Y';
			stageSelectAnim[4].flameFlag = 'Y';
			stageSelectAnim[4].selectNo = CHAR_SELECT_STAGE_B;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// ステージ５の位置にある時にEnter押したら、ゲームへ遷移（今は準備画面を表示）
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[7].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// TITLE位置にある時にEnter押したら、タイトルに戻る
				SetFade(FADE_OUT, MODE_TITLE);
				PlaySound(SOUND_LABEL_SE_defend001);
			}
		}

	}
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawStageSelect(void)
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


	// ステージセレクトの背景等を描画
	{
		for (int i = 0; i < STAGESELECT_MAX; i++)
		{
			/*[1]-------------------------------------------------------------*/
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelect[i].texNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(vertexBuffer,
				stageSelect[i].pos.x, stageSelect[i].pos.y, stageSelect[i].w, stageSelect[i].h,
				0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// セレクトアニメーションを描画
	{
		for (int i = 0; i < ANIM_MAX; i++)
		{
			if (stageSelectAnim[i].flameFlag == 'Y')		// この開くドアアニメーションが使われている？
			{// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelectAnim[i].texNo]);

				// プレイヤーの位置やテクスチャー座標を反映
				float px = stageSelectAnim[i].pos.x;	// 開くドアアニメーションの表示位置X
				float py = stageSelectAnim[i].pos.y;	// 開くドアアニメーションの表示位置Y
				float pw = stageSelectAnim[i].w;		// 開くドアアニメーションの表示幅
				float ph = stageSelectAnim[i].h;		// 開くドアアニメーションの表示高さ

				// アニメーション用
				float tw = 1.0f / ANIM_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / ANIM_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(stageSelectAnim[i].patternAnim % ANIM_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(stageSelectAnim[i].patternAnim / ANIM_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// 開くドアアニメーションを描画
	{
		for (int i = 0; i < DOOR_MAX; i++)
		{
			if (stageSelectDoor[i].flameFlag == 'N')		// この開くドアアニメーションが使われている？
			{									// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelectDoor[i].texNo]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = stageSelectDoor[i].pos.x;	// 開くドアアニメーションの表示位置X
				float py = stageSelectDoor[i].pos.y;	// 開くドアアニメーションの表示位置Y
				float pw = stageSelectDoor[i].w;		// 開くドアアニメーションの表示幅
				float ph = stageSelectDoor[i].h;		// 開くドアアニメーションの表示高さ

				// アニメーション用
				float tw = 1.0f / DOOR_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / DOOR_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else if (stageSelectDoor[i].flameFlag == 'Y' && stageSelectDoor[i].patternAnim < DOOR_ANIM_PATTERN_NUM)		// この開くドアアニメーションが使われている？
			{									// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelectDoor[i].texNo]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = stageSelectDoor[i].pos.x;	// 開くドアアニメーションの表示位置X
				float py = stageSelectDoor[i].pos.y;	// 開くドアアニメーションの表示位置Y
				float pw = stageSelectDoor[i].w;		// 開くドアアニメーションの表示幅
				float ph = stageSelectDoor[i].h;		// 開くドアアニメーションの表示高さ

				// アニメーション用
				float tw = 1.0f / DOOR_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / DOOR_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(stageSelectDoor[i].patternAnim % DOOR_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(stageSelectDoor[i].patternAnim / DOOR_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// 準備画面を描画
	{
		if (comingSoon.useFlag == 'Y')		// この開くドアアニメーションが使われている？
		{// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[comingSoon.texNo]);

			// プレイヤーの位置やテクスチャー座標を反映
			float px = comingSoon.pos.x;	// 開くドアアニメーションの表示位置X
			float py = comingSoon.pos.y;	// 開くドアアニメーションの表示位置Y
			float pw = comingSoon.w;		// 開くドアアニメーションの表示幅
			float ph = comingSoon.h;		// 開くドアアニメーションの表示高さ

			// アニメーション用
			float tw = 1.0f / COMINGSOON_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / COMINGSOON_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(comingSoon.patternAnim % COMINGSOON_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(comingSoon.patternAnim / COMINGSOON_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}