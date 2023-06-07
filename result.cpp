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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(22)				// �e�N�X�`���̐�

#define RANKING_DW					(128.0f)
#define RANKING_DH					(64.0f)
#define RANKING_DX					(100.0f)

#define PRESSENTER_DW				(512.0f)
#define PRESSENTER_DH				(128.0f)
#define PRESSENTER_DX				(SCREEN_CENTER_X - PRESSENTER_DW * 0.5f)
#define PRESSENTER_DY				(420.0f)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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


static BOOL		isLoad = FALSE;		// ���������s�������̃t���O
static RESULT	result;

static int		resultScore = 0;
static int		resultHighscore[HIGHSCORE_MAX];
static float	count;				// �_�ŗp�̃J�E���g
//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �ϐ��̏�����
	result.w     = TEXTURE_WIDTH;
	result.h     = TEXTURE_HEIGHT;
	result.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	result.texNo = 0;

	count = 0.0f;
	
	// �n�C�X�R�A�̃��[�h
	LoadHighscore();

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateResult(void)
{

	count += 0.05f;

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_TITLE);

		// SE
		PlaySound(SOUND_LABEL_SE_defend001);

	}
	// �Q�[���p�b�h�œ��͏���
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
// �`�揈��
//=============================================================================
void DrawResult(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[result.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(vertexBuffer,
			result.pos.x, result.pos.y, result.w, result.h, 
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �ŏI�X�R�A�\��
	{

		// ��������������
		int number = resultScore;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// ����\�����錅�̐���
			int x = (number % 10);
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[x + 1]);

			// ���̌���
			number /= 10;

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = SCORE_DX - SCORE_DW * i;	// �X�R�A�̕\���ʒuX
			float py = SCORE_DY;				// �X�R�A�̕\���ʒuY
			float pw = SCORE_DW;				// �X�R�A�̕\����
			float ph = SCORE_DH;				// �X�R�A�̕\������

			float tw = 1.0f / 1;				// �e�N�X�`���̕�
			float th = 1.0f / 1;				// �e�N�X�`���̍���
			float tx = 0.0f;					// �e�N�X�`���̍���X���W
			float ty = 0.0f;					// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(vertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

	}

	// �n�C�X�R�A�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[1]);

		for (int j = 0; j < HIGHSCORE_MAX; j++)
		{
			// ��������������
			int number = resultHighscore[j];

			for (int i = 0; i < SCORE_DIGIT; i++)
			{
				// ����\�����錅�̐���
				int x = number % 10;

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[x + 1]);

				// ���̌���
				number /= 10;

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = HIGHSCORE_DX - HIGHSCORE_DW * i + HIGHSCORE_LEN_X * (j / (HIGHSCORE_MAX / 2));	// �X�R�A�̕\���ʒuX
				float py = HIGHSCORE_DY + HIGHSCORE_LEN_Y * (j % (HIGHSCORE_MAX / 2));				// �X�R�A�̕\���ʒuY
				float pw = HIGHSCORE_DW;				// �X�R�A�̕\����
				float ph = HIGHSCORE_DH;				// �X�R�A�̕\������

				float tw = 1.0f;				// �e�N�X�`���̕�
				float th = 1.0f;				// �e�N�X�`���̍���
				float tx = 0.0f;					// �e�N�X�`���̍���X���W
				float ty = 0.0f;					// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(vertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}


		}

	}

	// ���ʂ̕\��
	{


		for (int i = 0; i < HIGHSCORE_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[i + 11]);


			// ���ʂ̈ʒu��e�N�X�`���[���W�𔽉f
			float px = RANKING_DX + HIGHSCORE_LEN_X * (i / (HIGHSCORE_MAX / 2));	// ���ʂ̕\���ʒuX
			float py = HIGHSCORE_DY + HIGHSCORE_LEN_Y * (i % (HIGHSCORE_MAX / 2));				// ���ʂ̕\���ʒuY
			float pw = RANKING_DW;					// ���ʂ̕\����
			float ph = RANKING_DH;					// ���ʂ̕\������

			float tw = 1.0f;						// �e�N�X�`���̕�
			float th = 1.0f;						// �e�N�X�`���̍���
			float tx = 0.0f;						// �e�N�X�`���̍���X���W
			float ty = 0.0f;						// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(vertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

	}

	// PRESS ENTER �̕\��
	{

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[21]);


		// �ʒu��e�N�X�`���[���W�𔽉f
		float px = PRESSENTER_DX;					// �\���ʒuX
		float py = PRESSENTER_DY;					// �\���ʒuY
		float pw = PRESSENTER_DW;					// �\����
		float ph = PRESSENTER_DH;					// �\������

		float tw = 1.0f;						// �e�N�X�`���̕�
		float th = 1.0f;						// �e�N�X�`���̍���
		float tx = 0.0f;						// �e�N�X�`���̍���X���W
		float ty = 0.0f;						// �e�N�X�`���̍���Y���W

		float a = 0.5f * (cosf(count) + 1.0f);
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(vertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, a));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}


}



//=============================================================================
// ���U���g�ŕ\������_���Z�b�g
//=============================================================================
void SetResult(int score)
{
	resultScore = score;

	// �n�C�X�R�A�����[�h����
	LoadHighscore();

	// �n�C�X�R�A�̕��ёւ�
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