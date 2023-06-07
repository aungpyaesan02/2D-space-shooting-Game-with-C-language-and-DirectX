//=============================================================================
//
//
//=============================================================================
#include "sprite.h"
#include "player.h"
#include "UI.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	BAR_WIDTH					(100)	// �^�C�g����ʂɂ���v���C���[�̃T�C�Y
#define	BAR_HEIGHT					(15)	// �^�C�g����ʂɂ���v���C���[�̃T�C�Y
#define	ICON_WIDTH					(25)	// �{�^���̃T�C�Y
#define	ICON_HEIGHT					(25)	// �{�^���̃T�C�Y

#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/HEALTH_UI/healthIcon.png",
	"data/TEXTURE/HEALTH_UI/bar.png",
};

static BOOL		isLoad = FALSE;		// ���������s�������̃t���O
static UI		ui[UI_MAX];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
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

	//UI�̏�����
	for (int i = 0; i < UI_MAX; i++)
	{
		// �A�C�R���̏�����
		ui[ICON].w = ICON_WIDTH;
		ui[ICON].h = ICON_HEIGHT;
		ui[ICON].pos = { 10.0f, SCREEN_HEIGHT - 30.0f, 0.0f };

		// �o�[�̏�����
		ui[BAR].w = BAR_WIDTH;
		ui[BAR].h = BAR_HEIGHT;
		ui[BAR].pos = { ui[ICON].pos.x + 25.0f, SCREEN_HEIGHT - 25.0f, 0.0f };

		ui[i].color = { 1.0f,1.0f,1.0f,1.0f };
		ui[i].texNo = i;
		
		ui[i].use = true;
	}

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	PLAYER* player = GetPlayer();
	
	// �v���C���[HP�ɂ��UI�i�o�[�j�̒����������
	ui[BAR].w = player->hp * 10;

	if (ui[BAR].w < 0.0f)
		ui[BAR].w = 0.0f;

#ifdef _DEBUG

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (ui[i].use)
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
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[ui[i].texNo]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(vertexBuffer,
					ui[i].pos.x, ui[i].pos.y, ui[i].w, ui[i].h,
					0.0f, 0.0f, 1.0f, 1.0f,
					ui[i].color);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
}






