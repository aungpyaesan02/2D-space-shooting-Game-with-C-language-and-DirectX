//=============================================================================
//
//
//=============================================================================
#include "score.h"
#include "sprite.h"
#include "file.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCORE_DW)	// �����̃T�C�Y
#define TEXTURE_HEIGHT				(SCORE_DH)	// 
#define TEXTURE_MAX					(10)			// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *texturName[] = {
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
};

static BOOL		load = FALSE;		// ���������s�������̃t���O
static SCORE	Score;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �ϐ��̏�����
	Score.w = TEXTURE_WIDTH;
	Score.h = TEXTURE_HEIGHT;
	Score.pos = D3DXVECTOR3(SCORE_DX, SCORE_DY, 0.0f);
	Score.texNo = 0;
	Score.score = 0;	// ���_��������
	

	load = TRUE;		// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
{
	if (load == FALSE) return;

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

	load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateScore(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[Score.texNo]);

	// ��������������
	int number = Score.score;

	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// ����\�����錅�̐���
		int x = (number % 10);
		Score.texNo = x;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[Score.texNo]);

		// ���̌���
		number /= 10;
		
		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = Score.pos.x - Score.w*i;	// �X�R�A�̕\���ʒuX
		float py = Score.pos.y;				// �X�R�A�̕\���ʒuY
		float pw = Score.w;					// �X�R�A�̕\����
		float ph = Score.h;					// �X�R�A�̕\������

		float tw = 1.0f / 1;					// �e�N�X�`���̕�
		float th = 1.0f / 1;					// �e�N�X�`���̍���
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


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddScore(int add)
{
	Score.score += add;
	if (Score.score > SCORE_MAX)
	{
		Score.score = SCORE_MAX;
	}

	// SE
	PlaySound(SOUND_LABEL_SE_score);

}


int GetScore(void)
{
	return Score.score;
}


void SetScore(int score)
{
	Score.score = score;
}

