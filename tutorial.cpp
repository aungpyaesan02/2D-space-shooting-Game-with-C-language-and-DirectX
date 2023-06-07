//=============================================================================
//

//
//=============================================================================
#include "tutorial.h"
#include "sprite.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(840)		// �����̃T�C�Y
#define TEXTURE_HEIGHT				(480) 
#define TEXTURE_MAX					(1)			// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/tutorial.jpg"
};

static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
TUTORIAL		tutorial;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
{
	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// �ϐ��̏�����
	tutorial.w = TEXTURE_WIDTH;
	tutorial.h = TEXTURE_HEIGHT;
	tutorial.texNo = 0;

	tutorial.use = TRUE;

	tutorial.pos = D3DXVECTOR3(60.0f, 30.0f, 0.0f);

	g_Load = TRUE;		// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTutorial(void)
{
	if (tutorial.use == TRUE)
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{			
			//PlaySound(SOUND_LABEL_SE_click);	���T�E���h�t�@�C�������������̂œK���ȓz�ɍ����ւ��܂����@����
			PlaySound(SOUND_LABEL_SE_button);

			SetFade(FADE_OUT, MODE_STAGE_SELECT);
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
{
	if (tutorial.use == TRUE)
	{
		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tutorial.texNo]);

		// �o�g�����O�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = tutorial.pos.x;				// �o�g�����O�̕\���ʒuX
		float py = tutorial.pos.y;				// �o�g�����O�̕\���ʒuY
		float pw = tutorial.w;					// �o�g�����O�̕\����
		float ph = tutorial.h;					// �o�g�����O�̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}

TUTORIAL *GetTutorial(void)
{
	return &tutorial;
}

