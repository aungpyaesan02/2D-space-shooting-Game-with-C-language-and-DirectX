//=============================================================================
//

//
//=============================================================================
#include "bg.h"
#include "mouseDraw.h"
#include "input.h"
#include "sprite.h"
#include "cursor.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "mousePad.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(5)		// �L�����T�C�Y�i���j
#define TEXTURE_HEIGHT				(5)		// �L�����T�C�Y�i�c�j
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *texturName[] = {
	"data/TEXTURE/MOUSE/drawPix_sample.png",
};

static BOOL	  isLoad = FALSE;				// ���������s�������̃t���O
static MOUSEDRAW mouseDraw[MOUSEDRAW_MAX];	// �}�E�X�`��\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitMouseDraw(void)
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


	// �}�E�X�`��\���̂̏�����
	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		mouseDraw[i].use   = FALSE;
		mouseDraw[i].w     = TEXTURE_WIDTH;
		mouseDraw[i].h     = TEXTURE_HEIGHT;
		mouseDraw[i].pos   = D3DXVECTOR3(300, 300.0f, 0.0f);
		mouseDraw[i].texNo = 0;

	}
	
	isLoad = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMouseDraw(void)
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
void UpdateMouseDraw(void)
{
	MOUSEPAD *mousePad = GetMousePad();

	int mousedrawCount = 0;				// ���������}�E�X�`��̐�

	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		// �}�E�X�`�揈��
		if (mouseDraw[i].use == TRUE)	// �}�E�X�`�悳��Ă�H
		{	// Yes
			// �}�E�X����p�p�b�h�̐��������蔻����s��
			for (int j = 0; j < MOUSEPAD_MAX; j++)
			{
				// �t���O�̗����ĂȂ��}�E�X����p�p�b�h�Ƃ̓����蔻�������
				if (mousePad[j].use == FALSE)
				{
					BOOL ans = CollisionBB(mouseDraw[i].pos, mouseDraw[i].w, mouseDraw[i].h,
						mousePad[j].pos, mousePad[j].w, mousePad[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						mousePad[j].use = TRUE;
						break;
					}
				}
			}
			mousedrawCount++;
		}

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMouseDraw(void)
{
	BG *bg = GetBG();
	MOUSEPAD *mousePad = GetMousePad();

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

	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		if (mouseDraw[i].use == TRUE)		// ���̃}�E�X�`�悪�g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[mouseDraw[i].texNo]);

			//�}�E�X�`��̈ʒu��e�N�X�`���[���W�𔽉f
			float px = mouseDraw[i].pos.x;	// �}�E�X�`��̕\���ʒuX
			float py = mouseDraw[i].pos.y;	// �}�E�X�`��̕\���ʒuY
			float pw = mouseDraw[i].w;		// �}�E�X�`��̕\����
			float ph = mouseDraw[i].h;		// �}�E�X�`��̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �}�E�X�`��\���̂̐擪�A�h���X���擾
//=============================================================================
MOUSEDRAW *GetMouseDraw(void)
{
	return &mouseDraw[0];
}


//=============================================================================
// �}�E�X�`��O�Ղ̕`��ݒ�
//=============================================================================
void SetMouseDraw(D3DXVECTOR3 pos)
{
	// �����}�E�X�`�悪����������`�悵�Ȃ�( =����ȏ㏑���Ȃ����Ď� )
	for (int i = 0; i < MOUSEDRAW_MAX; i++)
	{
		if (mouseDraw[i].use == FALSE)		// ���g�p��Ԃ̃}�E�X�`���������
		{
			mouseDraw[i].use = TRUE;		// �g�p��Ԃ֕ύX����
			mouseDraw[i].pos = pos;			// ���W���Z�b�g
			PlaySound(SOUND_LABEL_SE_mousepop);
			return;							// �P�`��Z�b�g�����̂ŏI������
		}
	}
}

