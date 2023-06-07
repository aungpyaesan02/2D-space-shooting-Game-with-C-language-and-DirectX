//=============================================================================
//
//
//=============================================================================
#include "bg.h"
#include "mousePad.h"
#include "mouseDraw.h"
#include "sprite.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(MOUSEPAD_DW)		// �}�E�X����p�p�b�h�̃T�C�Y�i���j
#define TEXTURE_HEIGHT				(MOUSEPAD_DH)		// �}�E�X����p�p�b�h�̃T�C�Y�i�c�j
#define TEXTURE_MAX					(1)					// �e�N�X�`���̐�
#define MOUSEPAD_INCLINATION		(0.785f)			// �}�E�X����p�p�b�h�̌X��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *texturName[TEXTURE_MAX] = {
	"data/TEXTURE/MOUSE/mousePad_sample.png",

};


static BOOL	isLoad = FALSE;					// ���������s�������̃t���O
static MOUSEPAD	mousePad[MOUSEPAD_MAX];		// �}�E�X����p�p�b�h�\����


// ����������
//=============================================================================
HRESULT InitMousePad(void)
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

	for (int i = 0; MOUSEPAD_MAX > i; i++)
	{
		// �ϐ��̏�����
		mousePad[i].use = FALSE;
		mousePad[i].w = TEXTURE_WIDTH;
		mousePad[i].h = TEXTURE_HEIGHT;
		mousePad[i].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY, 0.0f);
		mousePad[i].rot = D3DXVECTOR3(0.0f, 0.0f, MOUSEPAD_INCLINATION); // �΂߂ɂ���
		mousePad[i].texNo = 0;

	}

	// �}�E�X����p�p�b�h��ʌŔz�u

	/*
			�^0�_
		 �^1�_ �^2�_	
	  �^3�_ �^4�_ �^5�_
	  �_ �^6�_ �^7�_ �^
		 �_ �^8�_ �^
		    �_ �^
	*/

	// �O
	mousePad[0].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY - MOUSEPAD_DY1, 0.0f);
	// �P
	mousePad[1].pos = D3DXVECTOR3(MOUSEPAD_DX - MOUSEPAD_DX2, MOUSEPAD_DY - MOUSEPAD_DY2, 0.0f);
	// �Q
	mousePad[2].pos = D3DXVECTOR3(MOUSEPAD_DX + MOUSEPAD_DX2, MOUSEPAD_DY - MOUSEPAD_DY2, 0.0f);
	// �R
	mousePad[3].pos = D3DXVECTOR3(MOUSEPAD_DX - MOUSEPAD_DX1, MOUSEPAD_DY, 0.0f);
	// �S
	mousePad[4].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY, 0.0f);
	// �T
	mousePad[5].pos = D3DXVECTOR3(MOUSEPAD_DX + MOUSEPAD_DX1, MOUSEPAD_DY, 0.0f);
	// �U
	mousePad[6].pos = D3DXVECTOR3(MOUSEPAD_DX - MOUSEPAD_DX2, MOUSEPAD_DY + MOUSEPAD_DY2, 0.0f);
	// �V
	mousePad[7].pos = D3DXVECTOR3(MOUSEPAD_DX + MOUSEPAD_DX2, MOUSEPAD_DY + MOUSEPAD_DY2, 0.0f);
	// �X
	mousePad[8].pos = D3DXVECTOR3(MOUSEPAD_DX, MOUSEPAD_DY + MOUSEPAD_DY1, 0.0f);

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMousePad(void)
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
void UpdateMousePad(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMousePad(void)
{
	BG *bg = GetBG();

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

	for (int i = 0; MOUSEPAD_MAX > i; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[mousePad[i].texNo]);

		// �}�E�X����p�p�b�h�̔w�i��`��
		{
			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = mousePad[i].pos.x;	// �}�E�X����p�p�b�h�̕\���ʒuX
			float py = mousePad[i].pos.y;	// �}�E�X����p�p�b�h�̕\���ʒuY
			float pw = mousePad[i].w;		// �}�E�X����p�p�b�h�̕\����
			float ph = mousePad[i].h;		// �}�E�X����p�p�b�h�̕\������

			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���
			float tx = 0.0f;		// �e�N�X�`���̍���X���W
			float ty = 0.0f;		// �e�N�X�`���̍���Y���W

			// �}�E�X�`��Ɠ������Ă��Ȃ����͌��F
			if (mousePad[i].use == FALSE)
			{
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					mousePad[i].rot.z);
			}

			// �}�E�X�`��Ɠ������Ă��鎞�͐F��ς���
			if (mousePad[i].use == TRUE)
			{
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
					mousePad[i].rot.z);
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �\���̂̐擪�A�h���X���擾
//=============================================================================
MOUSEPAD *GetMousePad(void)
{
	return &mousePad[0];
}





