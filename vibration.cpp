//=============================================================================
//
//
//=============================================================================
#include "vibration.h"
#include "player.h"
#include "sprite.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(800)	// �h��\���T�C�Y�i���j
#define TEXTURE_HEIGHT				(800)	// �h��\���T�C�Y�i�c�j
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�
#define ANIM_WAIT					(80)	// �A�j���[�V�����̐؂�ւ��Wait�l
#define VIB_ADDITION_X				(1.0f)	// �h����Z�p�i���j
#define VIB_ADDITION_Y				(5.0f)	// �h����Z�p�i�c�j
#define VIB_MAX_X					(2.0f)	// �h�ꕝMax�i���j
#define VIB_MAX_Y					(10.0f)	// �h�ꕝMax�i�c�j

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/EFFECT/vibration_Damage.png",
};


static BOOL			isLoad = FALSE;		// ���������s�������̃t���O
static VIBRATION	vibration;			// �h��̍\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitVibration(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			textureName[i],
			NULL,
			NULL,
			&texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;			// 4���_��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �h��̍\���̂̏�����
	vibration.use = FALSE ;							// �����Ă��邩�ǂ���
	vibration.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
	vibration.w   = TEXTURE_WIDTH;					// ��
	vibration.h   = TEXTURE_HEIGHT;					// ����
	vibration.texNo = 0;							// �e�N�X�`���ԍ�

	vibration.countAnim = 0;						// �A�j���J�E���^�[(wait)

	isLoad = TRUE;									// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitVibration(void)
{
	if (isLoad == FALSE) return;			//���������Ă��Ȃ����I�����������Ȃ�

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
void UpdateVibration(void)
{

		// �g���Ă���h�ꂾ������������
		if (vibration.use == TRUE)
		{
			// �A�j���[�V����
			vibration.countAnim++;

			if (vibration.countAnim > ANIM_WAIT)
			{
				vibration.countAnim = 0;
				vibration.use = FALSE;
			}

			vibration.pos.x += VIB_ADDITION_X;
			if (vibration.pos.x > VIB_MAX_X)
			{
				vibration.pos.x = 0.0f;
			}

			vibration.pos.y += VIB_ADDITION_Y;
			if (vibration.pos.y > VIB_MAX_Y)
			{
				vibration.pos.y = 0.0f;
			}
		}

//#ifdef _DEBUG
//		// �f�o�b�O�\��
//		PrintDebugProc("BulletNo%d  X:%f Y:%f\n", i, vibration.pos.x, vibration.pos.y);
//#endif
	
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawVibration(void)
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

	if (vibration.use == TRUE)		// ���̗h�ꂪ�g���Ă���H
	{								// Yes
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[vibration.texNo]);

		//�h��̈ʒu��e�N�X�`���[���W�𔽉f
		float px = vibration.pos.x;				// �h��̕\���ʒuX
		float py = vibration.pos.y;				// �h��̕\���ʒuY
		float pw = vibration.w;					// �h��̕\����
		float ph = vibration.h;					// �h��̕\������


		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W			
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// �h��\���̂̐擪�A�h���X���擾
//=============================================================================
VIBRATION *GetVibration(void)
{
	return &vibration;
}


// �h����Z�b�g
void SetVibration()
{
	if (vibration.use == FALSE)
	{
		vibration.use = TRUE;
	}
}
