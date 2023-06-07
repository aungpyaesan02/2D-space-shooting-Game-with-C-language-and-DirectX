//=============================================================================

//
//=============================================================================
#include "main.h"
#include "bg.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "title.h"
#include "mousePad.h"
#include "mouseDraw.h"
#include "mouseSkill.h"
#include "cursor.h"
#include "player.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(1)		// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *texturName[TEXTURE_MAX] = {

};

static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ

static CURSOR	cursor;				//カーソル構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCursor(void)
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
	cursor.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	cursor.use = FALSE;


	isLoad = TRUE;				// データの初期化を行った
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCursor(void)
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
void UpdateCursor(void)
{
	MOUSESKILL *mouseSkill = GetMouseSkill();
	MOUSEPAD *mousePad = GetMousePad();
	MOUSEDRAW *mouseDraw = GetMouseDraw();

	// マウスの座標をカーソル構造体へ代入している
	cursor.pos.x = (float)GetMousePosX();
	cursor.pos.y = (float)GetMousePosY();

	// カーソル左クリックで筆のドロー処理
	if (IsMouseLeftPressed())
	{// Enter押したら、ステージを切り替える
		D3DXVECTOR3 pos = D3DXVECTOR3((float)GetMousePosX() , (float)GetMousePosY(), 0.0f);
		SetMouseDraw(pos);		// １発目

	}

	// カーソル右クリックで筆のドローと条件によってスキル発動
	
	MouseSkillSet();	// ← この関数内に条件詳細記載

#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Cursor X:%f \n", cursor.pos.x);
	PrintDebugProc("Cursor Y:%f \n", cursor.pos.y);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCursor(void)
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
}


//=============================================================================
// テクスチャの座標上にカーソルがあるか取得する関数
//=============================================================================
BOOL CheckCursorPos(float x, float y, float w, float h)
{
	// テクスチャ（文字の部分）の座標上にカーソルがあるか?
	if (   x <= (float)GetMousePosX()
		&& y <= (float)GetMousePosY()
		&& w >= (float)GetMousePosX()
		&& h >= (float)GetMousePosY())
	{
		// あったらTRUEを代入
		cursor.use = TRUE;
	}
	else
	{
		// ない時はFALSEを代入
		return cursor.use = FALSE;
	}

	// 代入した値を返す
	return cursor.use;

}


//=============================================================================
// マウススキルのセット
//=============================================================================
void MouseSkillSet(void)
{
	PLAYER *player = GetPlayer();
	MOUSESKILL *mouseSkill = GetMouseSkill();
	MOUSEPAD *mousePad = GetMousePad();
	MOUSEDRAW *mouseDraw = GetMouseDraw();player->pos;

	// 条件によってマウス右クリックを押したら、マウススキル発動
	// マウススキルと筆のドローキャンセル
	for (int i = 0; MOUSEDRAW_MAX > i; i++)
	{
		for (int j = 0; MOUSEPAD_MAX > j; j++)
		{
			/* ここにマウス判定用パッドの当たり判定状態と右クリックに応じたスキルを追加していく*/


			/* SET_SKILL_RIGHTSHOT */
			/*
					／0＼
					＼ ／2＼
					   ＼ ／5＼
					   ／7＼ ／
					／8＼ ／
					＼ ／
			*/
			if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == FALSE &&
				mousePad[2].use == TRUE  && mousePad[3].use == FALSE &&
				mousePad[4].use == FALSE && mousePad[5].use == TRUE  &&
				mousePad[6].use == FALSE && mousePad[7].use == TRUE  &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos;	// プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_RIGHTSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE;	// 未使用（使用されていないマウス判定用パッド状態にする）
				}
			}


			/* SET_SKILL_LEFTSHOT */
			/*
					／0＼
				 ／1＼ ／
			  ／3＼ ／
			  ＼ ／6＼
				 ＼ ／8＼
					＼ ／
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == TRUE  &&
				mousePad[2].use == FALSE && mousePad[3].use == TRUE  &&
				mousePad[4].use == FALSE && mousePad[5].use == FALSE &&
				mousePad[6].use == TRUE  && mousePad[7].use == FALSE &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_LEFTSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）
				}
			}


			/* SET_SKILL_UPSHOT */
			/*
					／0＼
				 ／1＼ ／2＼
			  ／3＼ ／ ＼ ／5＼
			  ＼ ／		  ＼ ／


			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == TRUE  &&
				mousePad[2].use == TRUE  && mousePad[3].use == TRUE  &&
				mousePad[4].use == FALSE && mousePad[5].use == TRUE  &&
				mousePad[6].use == FALSE && mousePad[7].use == FALSE &&
				mousePad[8].use == FALSE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_UPSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）
				}
			}


			/* SET_SKILL_DOWNSHOT */
			/*


			  ／3＼		  ／5＼
			  ＼ ／6＼ ／7＼ ／
				 ＼ ／8＼ ／
					＼ ／
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == FALSE && mousePad[1].use == FALSE &&
				mousePad[2].use == FALSE && mousePad[3].use == TRUE  &&
				mousePad[4].use == FALSE && mousePad[5].use == TRUE  &&
				mousePad[6].use == TRUE  && mousePad[7].use == TRUE  &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_DOWNSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）
				}
			}


			/* SET_SKILL_CROSS */
			/*

				 ／1＼ ／2＼
				 ＼ ／4＼ ／
				 ／6＼ ／7＼
				 ＼ ／ ＼ ／

			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == FALSE && mousePad[1].use == TRUE  &&
				mousePad[2].use == TRUE  && mousePad[3].use == FALSE &&
				mousePad[4].use == TRUE  && mousePad[5].use == FALSE &&
				mousePad[6].use == TRUE  && mousePad[7].use == TRUE  &&
				mousePad[8].use == FALSE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_CROSS;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）


					// SE
					PlaySound(SOUND_LABEL_SE_mousepop);
				}
			}


			/* SET_SKILL_STUN */
			/*
					／0＼
				 ／1＼ ／
				 ＼ ／4＼
					＼ ／7＼
					／8＼ ／
					＼ ／
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == TRUE  &&
				mousePad[2].use == FALSE && mousePad[3].use == FALSE &&
				mousePad[4].use == TRUE  && mousePad[5].use == FALSE &&
				mousePad[6].use == FALSE && mousePad[7].use == TRUE  &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_STUN;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）
				}
			}


			/* SET_SKILL_HOMING */
			/*
					／0＼
				 ／1＼ ／2＼
				 ＼ ／4＼ ／
				 ／6＼ ／7＼
				 ＼ ／ ＼ ／

			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE && mousePad[1].use == TRUE  &&
				mousePad[2].use == TRUE && mousePad[3].use == FALSE &&
				mousePad[4].use == TRUE && mousePad[5].use == FALSE &&
				mousePad[6].use == TRUE && mousePad[7].use == TRUE  &&
				mousePad[8].use == FALSE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_HOMING;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）
				}

				// SE

			}


			/* SET_SKILL_VORTEX */
			/*
					／0＼
				 ／1＼ ／2＼
			  ／3＼ ／4＼ ／5＼
			  ＼ ／6＼ ／7＼ ／
				 ＼ ／8＼ ／
					＼ ／
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE && mousePad[1].use == TRUE &&
				mousePad[2].use == TRUE && mousePad[3].use == TRUE &&
				mousePad[4].use == TRUE && mousePad[5].use == TRUE &&
				mousePad[6].use == TRUE && mousePad[7].use == TRUE &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動
					D3DXVECTOR3 pos = player->pos; // プレイヤーのポジションを引数にする。
					int skill = SET_SKILL_VORTEX;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）

					// SE
					PlaySound(SOUND_LABEL_SE_mousepop);

				}
			}


			/* SET_SKILL_追加用 */
			/*
					／0＼
				 ／1＼ ／2＼
			  ／3＼ ／4＼ ／5＼
			  ＼ ／6＼ ／7＼ ／
				 ＼ ／8＼ ／
					＼ ／
			*/


			else
			{
				if (IsMouseRightTriggered())
				{// この条件でマウス右クリックを押したら、マウススキル発動はしない
					mouseDraw[i].use = FALSE;	// 未使用（描画されていないマウス軌跡にする）
					mousePad[j].use = FALSE; // 未使用（使用されていないマウス判定用パッド状態にする）
				}
			}
		}
	}
}