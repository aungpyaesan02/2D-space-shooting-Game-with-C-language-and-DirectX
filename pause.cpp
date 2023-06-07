//=============================================================================
//
//
//=============================================================================
#include "pause.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PAUSE_MENU_WIDTH			(200)
#define PAUSE_MENU_HEIGHT			(PAUSE_MENU_WIDTH / 4)
#define PAUSE_MENU_OFFSET_Y			(50)

#define TEXTURE_WIDTH				(SCREEN_WIDTH)			// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)			// 
#define TEXTURE_MAX					(1 + PAUSE_MENU_MAX)	// �e�N�X�`���̐�(�w�i�{���j���[)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/BG/bg_pause.png",
	"data/TEXTURE/BG/pause_resume.png",
	"data/TEXTURE/BG/pause_restart.png",
	"data/TEXTURE/BG/pause_title.png",
};


static BOOL			isLoad = FALSE;		// ���������s�������̃t���O
static PAUSE		pause;
static PAUSE_MENU	menu[PAUSE_MENU_MAX];

static int curMenuNum;					// ���ݑI�����Ă��郁�j���[�̔ԍ�

static D3DXCOLOR defaultColor;			// ��I�����̃��j���[�F
static D3DXCOLOR selectedColor;			// �I�����̃��j���[�F
//=============================================================================
// ����������
//=============================================================================
HRESULT InitPause(void)
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
	pause.w     = TEXTURE_WIDTH;
	pause.h     = TEXTURE_HEIGHT;
	pause.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pause.texNo = 0;

	float leftX = SCREEN_CENTER_X - PAUSE_MENU_WIDTH * PAUSE_MENU_MAX * 0.5f;		// ���j���[�摜�̍��[���W
	for (int i = 0; i < PAUSE_MENU_MAX; i++)
	{
		menu[i].w = PAUSE_MENU_WIDTH;
		menu[i].h = PAUSE_MENU_HEIGHT;
		menu[i].pos = D3DXVECTOR3(leftX + PAUSE_MENU_WIDTH * i, SCREEN_CENTER_Y + PAUSE_MENU_OFFSET_Y, 0.0f);
		menu[i].texNo = i + 1;
		menu[i].selected = false;
	}

	curMenuNum = 0;
	menu[curMenuNum].selected = false;					// �ŏ��ɑI�����Ă������j���[

	defaultColor = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.7f);	// �I�����Ă��Ȃ��Ƃ��͈Â߂̐F
	selectedColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	SetPause(false);	// �|�[�Y��ԏ�����

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPause(void)
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
void UpdatePause(void)
{
	// F1�L�[�Ń|�[�Y��Ԑ؂�ւ�
	if (GetKeyboardTrigger(DIK_F1))
	{
		SetPause(!GetPause());
		// SE
		PlaySound(SOUND_LABEL_SE_button);
	}

	// ���j���[�I��
	if (GetKeyboardTrigger(DIK_LEFTARROW))
	{
		curMenuNum--;
		// SE
		PlaySound(SOUND_LABEL_SE_button);

	}
	else if (GetKeyboardTrigger(DIK_RIGHTARROW))
	{
		curMenuNum++;
		// SE
		PlaySound(SOUND_LABEL_SE_button);

	}

	// �[�܂ōs�����甽�Α���
	if (curMenuNum < 0)
	{
		curMenuNum = PAUSE_MENU_MAX - 1;
	}
	else if (curMenuNum >= PAUSE_MENU_MAX)
	{
		curMenuNum = 0;
	}

	// ��������S���I������
	for (int i = 0; i < PAUSE_MENU_MAX; i++)
	{
		menu[i].selected = false;
	}

	// �I��
	menu[curMenuNum].selected = true;

	// �|�[�Y���ɃG���^�[�L�[����������
	if (GetKeyboardTrigger(DIK_RETURN) && GetPause() == TRUE)
	{
		switch (curMenuNum)
		{
		case PAUSE_MENU_RESUME:		// ������
			SetPause(false);
			break;

		case PAUSE_MENU_RESTART:	// ���X�^�[�g
			// ���X�^�[�g����
			break;

		case PAUSE_MENU_TITLE:		// �^�C�g��
			SetFade(FADE_OUT, MODE_TITLE);
			break;
		}

		PlaySound(SOUND_LABEL_SE_button);

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPause(void)
{
	// �|�[�Y������Ȃ���Ε`�悵�Ȃ�
	if (GetPause() == FALSE) return;

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

	// �|�[�Y��ʕ`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(vertexBuffer,
			pause.pos.x, pause.pos.y, pause.w, pause.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �|�[�Y���j���[�`��
	for (size_t i = 0; i < PAUSE_MENU_MAX; i++)
	{
		// �I�����Ă��邩�ǂ����ŐF��ύX
		D3DXCOLOR color;
		menu[i].selected ? color = selectedColor : color = defaultColor;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[i + 1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(vertexBuffer,
			menu[i].pos.x, menu[i].pos.y, menu[i].w, menu[i].h,
			0.0f, 0.0f, 1.0f, 1.0f,
			color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}
