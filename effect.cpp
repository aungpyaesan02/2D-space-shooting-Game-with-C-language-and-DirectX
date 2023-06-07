//=============================================================================

//
//=============================================================================
#include "effect.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "player.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[EFFECT_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[EFFECT_MAX] = {
	"data/TEXTURE/EFFECT/effect_explosion.png",
};


static BOOL			isLoad = FALSE;			// ���������s�������̃t���O
static EFFECT		effect[EFFECT_MAX];		// �G�t�F�N�g�\����
static EFFECT_DATA	effectData[EFFECT_DATA_MAX]{
	// tex, divideX, divideY
	{ 0, 5, 1 },	// EXPLOSION
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < EFFECT_MAX; i++)
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


	// �G�t�F�N�g�\���̂̏�����
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		effect[i].use = FALSE;
		effect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		effect[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		effect[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		effect[i].countAnim = 0;
		effect[i].patternAnim = 0;
	}

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
{
	if (isLoad == FALSE) return;

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	for (int i = 0; i < EFFECT_MAX; i++)
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
void UpdateEffect(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_F2))
	{
		SetEffect(EFFECT_EXPLOSION, D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f), 100, 100);
	}
#endif // DEBUG


	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == TRUE)
		{
			// �A�j���[�V����  
			effect[i].countAnim++;
			if (effect[i].countAnim >= ANIM_WAIT)
			{
				effect[i].countAnim = 0;

				// �p�^�[���̐؂�ւ�
				effect[i].patternAnim++;
				int patternMax = effect[i].data->texDivideX * effect[i].data->texDivideY;

				// �Đ����I������玩���ŏ�����
				if (effect[i].patternAnim >= patternMax)
				{
					effect[i].use = false;
				}
			}
		}
	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
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

	BG *bg = GetBG();

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == TRUE)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[effect[i].data->texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = effect[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = effect[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = effect[i].w;					// �G�l�~�[�̕\����
			float ph = effect[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f / effect[i].data->texDivideX;	// �e�N�X�`���̕�
			float th = 1.0f / effect[i].data->texDivideY;	// �e�N�X�`���̍���
			float tx = (float)(effect[i].patternAnim % effect[i].data->texDivideX) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(effect[i].patternAnim / effect[i].data->texDivideY) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				effect[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �G�l�~�[�\���̂̐擪�A�h���X���擾
//=============================================================================
void SetEffect(int effectIndex, D3DXVECTOR3 pos, float w, float h)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == FALSE)
		{
			effect[i].use = TRUE;
			effect[i].pos = pos;
			effect[i].w = w;
			effect[i].h = h;
			effect[i].countAnim = 0;
			effect[i].patternAnim = 0;
			effect[i].data = &effectData[effectIndex];
			break;
		}
	}

}




