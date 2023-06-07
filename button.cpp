//=============================================================================
//

//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "button.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_WIDTH				(100)	// タイトル画面にあるプレイヤーのサイズ
#define PLAYER_HEIGHT				(100)	// タイトル画面にあるプレイヤーのサイズ
#define	BUTTON_WIDTH				(120)	// ボタンのサイズ
#define	BUTTON_HEIGHT				(50)	// ボタンのサイズ
#define	TITLE_WIDTH					(700)	// タイトルのサイズ
#define	TITLE_HEIGHT				(150)	// タイトルのサイズ

#define TEXTURE_MAX					(BUTTON_MAX)		// テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/Start_BTN.png",
	"data/TEXTURE/BG/Exit_BTN.png",
	"data/TEXTURE/PLAYER/ship_001.png",
	"data/TEXTURE/BG/title.png",
};

static BOOL		isLoad = FALSE;		// 初期化を行ったかのフラグ
static BUTTON	button[BUTTON_MAX];
static BOOL		eventStart = FALSE;
static int		cursorPosition;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitButton(void)
{
	ID3D11Device *pDevice = GetDevice();
	float buttonOffset = 60.0f;
	cursorPosition = 0;

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

	//Buttonの初期化
	for (int i = 0; i < BUTTON_MAX; i++)
	{
		button[i].w = BUTTON_WIDTH;
		button[i].h = BUTTON_HEIGHT;
		button[i].pos = { (SCREEN_WIDTH * 0.5f - BUTTON_WIDTH * 0.5f), SCREEN_HEIGHT * 0.5f + buttonOffset + (buttonOffset*i), 0.0f };
		button[i].texNo = i;
		
		// 色の設定
		{
			if (button[i].texNo == cursorPosition || button[i].texNo == PLAYER_BUTTON || button[i].texNo == TITLE_LOGO)
				button[i].color = { 1.0f,1.0f,1.0f,1.0f };
			else
				button[i].color = { 0.5f,0.5f,0.5f,1.0f };
		}

		button[i].use = true;
	}

	// タイトル画面にあるプレイヤーの初期化
	button[PLAYER_BUTTON].w = PLAYER_WIDTH;
	button[PLAYER_BUTTON].h = PLAYER_HEIGHT;
	button[PLAYER_BUTTON].pos = { (SCREEN_WIDTH * 0.75f), (SCREEN_HEIGHT * 0.7f), 0.0f };

	// タイトルロゴの初期化
	button[TITLE_LOGO].w = TITLE_WIDTH;
	button[TITLE_LOGO].h = TITLE_HEIGHT;
	button[TITLE_LOGO].pos = { (SCREEN_WIDTH * 0.5f - TITLE_WIDTH*0.5f), (SCREEN_HEIGHT * 0.25f), 0.0f };

	isLoad = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitButton(void)
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
void UpdateButton(void)
{
	if (!eventStart)
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			switch (cursorPosition)
			{
			case START_BUTTON:
				for (int i = 0; i < BUTTON_MAX; i++)
				{
					if (button[i].texNo != PLAYER_BUTTON && button[i].texNo != TITLE_LOGO) button[i].use = false;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_gamesave);

				eventStart = TRUE;	// イベントのスタートフラグをONにする
				break;

			case EXIT_BUTTON:
				exit(0);
				break;

			default:
				break;
			}
			// SE

			PlaySound(SOUND_LABEL_SE_button);

		}

		// メニュー選択
		if (GetKeyboardTrigger(DIK_UPARROW))
		{
			cursorPosition = -(cursorPosition - 1) % (BUTTON_MAX);
			// SE
			PlaySound(SOUND_LABEL_SE_button);

		}
		else if (GetKeyboardTrigger(DIK_DOWNARROW))
		{
			cursorPosition = (cursorPosition + 1) % (BUTTON_MAX - 2);
			// SE
			PlaySound(SOUND_LABEL_SE_button);

		}

		SelectMenu();

	}
	else
	{
		GameStartAnimation();
	}

#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Cursor Position : %d \n", cursorPosition);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawButton(void)
{
	for (int i = 0; i < BUTTON_MAX; i++)
	{
		if (button[i].use)
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

			// タイトルの背景を描画
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[button[i].texNo]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(vertexBuffer,
					button[i].pos.x, button[i].pos.y, button[i].w, button[i].h,
					0.0f, 0.0f, 1.0f, 1.0f,
					button[i].color);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
}

// タイトル画面にあるアニメーション処理
void GameStartAnimation(void)
{
	float spd = 4.0f;	// 移動量
	float fadeRate = 0.03f;

	if (button[PLAYER_BUTTON].pos.x <= (SCREEN_WIDTH * 0.5f) - (PLAYER_WIDTH * 0.5f))
	{
		button[PLAYER_BUTTON].pos.y -= spd * 3.0f;
		button[TITLE_LOGO].use = false;

		if (button[PLAYER_BUTTON].pos.y < 0.0f - PLAYER_WIDTH)
		{
			button[PLAYER_BUTTON].use = false;
			eventStart = FALSE;
			SetFade(FADE_OUT, MODE_TUTORIAL);
		}
	}
	else
	{
		button[PLAYER_BUTTON].pos.x -= spd;
		button[TITLE_LOGO].color.a -= fadeRate;
	}
}

void SelectMenu(void)
{
	for (int i = 0; i < BUTTON_MAX; i++)
	{
		if (button[i].texNo == cursorPosition || button[i].texNo == PLAYER_BUTTON || button[i].texNo == TITLE_LOGO)
			button[i].color = { 1.0f,1.0f,1.0f,1.0f };
		else
			button[i].color = { 0.5f,0.5f,0.5f,1.0f };
	}
}





