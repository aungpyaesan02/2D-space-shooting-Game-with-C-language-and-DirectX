//=============================================================================
//
//
//=============================================================================
#include "bg.h"
#include "sprite.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(2048.0f)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(2048.0f)	// �w�i�T�C�Y
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/bg_stage01.png",
};


static BOOL	isLoad = FALSE;		// ���������s�������̃t���O
static BG	bg;					// BG�\����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �ϐ��̏�����
	bg.w     = TEXTURE_WIDTH;
	bg.h     = TEXTURE_HEIGHT;
	bg.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bg.texNo = 0;

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	// �}�b�v�X�N���[��
	PLAYER *player = GetPlayer();

	bg.pos.x = player[0].pos.x - PLAYER_DISP_X;
	if (bg.pos.x < 0) bg.pos.x = 0;
	if (bg.pos.x > (bg.w - SCREEN_WIDTH)) bg.pos.x = bg.w - SCREEN_WIDTH;

	bg.pos.y = player[0].pos.y - PLAYER_DISP_Y;
	if (bg.pos.y < 0) bg.pos.y = 0;
	if (bg.pos.y > bg.h - SCREEN_HEIGHT) bg.pos.y = bg.h - SCREEN_HEIGHT;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

	// BG�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[bg.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(vertexBuffer,
			-bg.pos.x, -bg.pos.y, bg.w, bg.h, 
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG *GetBG(void)
{
	return &bg;
}