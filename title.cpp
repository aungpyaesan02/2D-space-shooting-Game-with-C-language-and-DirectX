//=============================================================================
//
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(2)				// �e�N�X�`���̐�

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/BG.png",
	"data/TEXTURE/BG/Stars.png",
};


static BOOL		isLoad = FALSE;		// ���������s�������̃t���O
static TITLE	title[TEXTURE_MAX];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		// �ϐ��̏�����
		title[i].w = TEXTURE_WIDTH;
		title[i].h = TEXTURE_HEIGHT;
		title[i].u = title[i].v = 0.0f;
		title[i].uw = title[i].vh = 1.0f;
		title[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		title[i].texNo = i;
	}

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	title[LAYER_2].v -= 0.005f;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
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

		// �^�C�g���̔w�i��`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[title[i].texNo]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(vertexBuffer,
				title[i].pos.x, title[i].pos.y, title[i].w, title[i].h,
				title[i].u, title[i].v, title[i].uw, title[i].vh,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}





