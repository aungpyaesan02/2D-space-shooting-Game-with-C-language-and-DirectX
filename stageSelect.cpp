//=============================================================================
//

//
//=============================================================================
#include "stageSelect.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(9)				// �e�N�X�`���̐�

#define STAGESELECT_BG_WIDTH		(SCREEN_WIDTH)	// �X�e�[�W�Z���N�g�w�i�T�C�Y�i�c�j
#define STAGESELECT_BG_HEIGHT		(SCREEN_HEIGHT)	// �X�e�[�W�Z���N�g�w�i�T�C�Y�i���j
#define STAGESELECT_FLAME_WIDTH		(128)			// �X�e�[�W�Z���N�g�w�i�T�C�Y�i�c�j
#define STAGESELECT_FLAME_HEIGHT	(128)			// �X�e�[�W�Z���N�g�w�i�T�C�Y�i���j
#define ADJUSUMENT_WIDTH			(STAGESELECT_FLAME_WIDTH / 2)	// �w�i�����p�i�c�j
#define ADJUSUMENT_HEIGHT			(STAGESELECT_FLAME_HEIGHT / 2)	// �w�i�����p�i���j
#define COMINGSOON_WIDTH			(540)			// ������ʁi�c�j
#define COMINGSOON_HEIGHT			(230)			// ������ʁi���j

#define CHOICE_MAX					(7)				// �I������Max��

#define ANIM_PATTERN_DIVIDE_X		(5)				// �Z���N�g�A�j���[�V�����̃e�N�X�`�����������iX)
#define ANIM_PATTERN_DIVIDE_Y		(5)				// �Z���N�g�A�j���[�V�����̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(ANIM_PATTERN_DIVIDE_X*ANIM_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(12)			// �Z���N�g�A�j���[�V�����̐؂�ւ��Wait�l

#define DOOR_PATTERN_DIVIDE_X		(5)				// �J���h�A�A�j���[�V�����̃e�N�X�`�����������iX)
#define DOOR_PATTERN_DIVIDE_Y		(1)				// �J���h�A�A�j���[�V�����̃e�N�X�`�����������iY)
#define DOOR_ANIM_PATTERN_NUM		(DOOR_PATTERN_DIVIDE_X*DOOR_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define DOOR_ANIM_WAIT				(12)			// �h�A�A�j���[�V�����̐؂�ւ��Wait�l

#define COMINGSOON_PATTERN_DIVIDE_X	(3)				// �J���h�A�A�j���[�V�����̃e�N�X�`�����������iX)
#define COMINGSOON_PATTERN_DIVIDE_Y	(1)				// �J���h�A�A�j���[�V�����̃e�N�X�`�����������iY)
#define COMINGSOON_ANIM_PATTERN_NUM	(COMINGSOON_PATTERN_DIVIDE_X*COMINGSOON_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define COMINGSOON_ANIM_WAIT		(16)			// �h�A�A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/STAGE_SELECT/select_Stage.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_BG.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame_Status.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame_Base.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame_Title.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Flame.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Door.png",
	"data/TEXTURE/STAGE_SELECT/select_Stage_Anim.png",
	"data/TEXTURE/STAGE_SELECT/comingSoon.png",

};


static BOOL		isLoad = FALSE;		// ���������s�������̃t���O
static STAGESELECT	stageSelect[STAGESELECT_MAX];
static STAGESELECT_FLAME	stageSelectFlame[CHOICE_MAX];
static STAGESELECT_DOOR		stageSelectDoor[DOOR_MAX];			// �J���h�A�A�j���[�V�����p�\����
static STAGESELECT_ANIM		stageSelectAnim[ANIM_MAX];			// �X�e�[�W�A�j���[�V�����p�\����
static COMINGSOON			comingSoon;			// �X�e�[�W�A�j���[�V�����p�\����

static D3DXVECTOR3 selectFlame[CHOICE_MAX] =
{
	D3DXVECTOR3(120.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(240.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(360.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(SCREEN_CENTER_X - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(600.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(720.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f),
	D3DXVECTOR3(840.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f),
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitStageSelect(void)
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


	// �X�e�[�W�Z���N�g�w�i���̏�����
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		stageSelect[i].w = STAGESELECT_BG_WIDTH;
		stageSelect[i].h = STAGESELECT_BG_HEIGHT;
		stageSelect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	stageSelect[0].texNo = 1;

	stageSelect[1].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[1].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[1].pos = D3DXVECTOR3(120.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[1].texNo = 2;

	stageSelect[2].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[2].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[2].pos = D3DXVECTOR3(240.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[2].texNo = 3;

	stageSelect[3].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[3].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[3].pos = D3DXVECTOR3(360.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[3].texNo = 3;

	stageSelect[4].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[4].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[4].pos = D3DXVECTOR3(SCREEN_CENTER_X - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[4].texNo = 3;

	stageSelect[5].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[5].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[5].pos = D3DXVECTOR3(600.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[5].texNo = 3;

	stageSelect[6].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[6].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[6].pos = D3DXVECTOR3(720.0f - ADJUSUMENT_WIDTH, SCREEN_CENTER_Y - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[6].texNo = 3;

	stageSelect[7].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[7].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[7].pos = D3DXVECTOR3(840.0f - ADJUSUMENT_WIDTH, 405.0f - ADJUSUMENT_HEIGHT, 0.0f);
	stageSelect[7].texNo = 4;

	stageSelect[8].w = STAGESELECT_FLAME_WIDTH;
	stageSelect[8].h = STAGESELECT_FLAME_HEIGHT;
	stageSelect[8].pos = stageSelect[1].pos;
	stageSelect[8].texNo = 5;

	stageSelectFlame[0].flameSw = 0;	//�g���W�̐؂�ւ��p


	// �Z���N�g�A�j���[�V�����̏�����
	for (int i = 0; i < ANIM_MAX; i++)
	{
		stageSelectAnim[i].flameFlag = 'N';
		stageSelectAnim[i].w = STAGESELECT_FLAME_WIDTH;
		stageSelectAnim[i].h = STAGESELECT_FLAME_HEIGHT;
		stageSelectAnim[i].texNo = 7;
		stageSelectAnim[i].countAnim = 0;
		stageSelectAnim[i].patternAnim = 0;
		stageSelectAnim[i].selectNo = CHAR_SELECT_STAGE_B;
		stageSelectAnim[i].patternAnim = stageSelectAnim[i].selectNo * ANIM_PATTERN_DIVIDE_X;

	}

	stageSelectAnim[0].pos = D3DXVECTOR3(240.0f, SCREEN_CENTER_Y, 0.0f);
	stageSelectAnim[1].pos = D3DXVECTOR3(360.0f, 405.0f, 0.0f);
	stageSelectAnim[2].pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	stageSelectAnim[3].pos = D3DXVECTOR3(600.0f, 405.0f, 0.0f);
	stageSelectAnim[4].pos = D3DXVECTOR3(720.0f, SCREEN_CENTER_Y, 0.0f);

	// �J���h�A�A�j���[�V�����̏�����
	for (int i = 0; i < DOOR_MAX; i++)
	{
		stageSelectDoor[i].flameFlag = 'N';
		stageSelectDoor[i].w = STAGESELECT_FLAME_WIDTH;
		stageSelectDoor[i].h = STAGESELECT_FLAME_HEIGHT;
		stageSelectDoor[i].texNo = 6;
		stageSelectDoor[i].countAnim = 0;
		stageSelectDoor[i].patternAnim = 0;
	}

	stageSelectDoor[0].pos = D3DXVECTOR3(240.0f, SCREEN_CENTER_Y, 0.0f);
	stageSelectDoor[1].pos = D3DXVECTOR3(360.0f, 405.0f, 0.0f);
	stageSelectDoor[2].pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	stageSelectDoor[3].pos = D3DXVECTOR3(600.0f, 405.0f, 0.0f);
	stageSelectDoor[4].pos = D3DXVECTOR3(720.0f, SCREEN_CENTER_Y, 0.0f);

	// ������ʂ̏�����
	comingSoon.useFlag = 'N';
	comingSoon.w = COMINGSOON_WIDTH;
	comingSoon.h = COMINGSOON_HEIGHT;
	comingSoon.pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	comingSoon.texNo = 8;
	comingSoon.countAnim = 0;
	comingSoon.patternAnim = 0;

	isLoad = TRUE;						// �f�[�^�̏��������s����
	return S_OK;

}


//=============================================================================
// �I������
//=============================================================================
void UninitStageSelect(void)
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
void UpdateStageSelect(void)
{
	// �h�A�A�j���[�V����
	{

		for (int i = 0; i < DOOR_MAX; i++)
		{
			// �t���O�ɂ���ăA�j���[�V�����̗L�������߂�
			switch (stageSelectDoor[i].flameFlag)
			{
			case 'Y':
				stageSelectDoor[i].countAnim += 1.0f;
				if (stageSelectDoor[i].countAnim > DOOR_ANIM_WAIT)
				{
					stageSelectDoor[i].countAnim = 0.0f;
					// �A�j���p�^�[���̐؂�ւ�
					stageSelectDoor[i].patternAnim++;
				}
				break;

			case 'N':
				break;

			default:
				break;
			}
		}

	}

	// �Z���N�g�A�j���[�V����
	{

		for (int i = 0; i < ANIM_MAX; i++)
		{
			// �t���O�ɂ���ăA�j���[�V�����̗L�������߂�
			switch (stageSelectAnim[i].flameFlag)
			{
			case 'Y':
				stageSelectAnim[i].countAnim += 1.0f;
				if (stageSelectAnim[i].countAnim > ANIM_WAIT)
				{
					stageSelectAnim[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					stageSelectAnim[i].patternAnim = (stageSelectAnim[i].selectNo * ANIM_PATTERN_DIVIDE_X) + ((stageSelectAnim[i].patternAnim + 1) % ANIM_PATTERN_DIVIDE_X);
				}
				break;
			case 'N':
				break;

			default:
				break;
			}
		}

	}

	// ������ʃA�j���[�V����
	{
		switch (comingSoon.useFlag)
		{
		case 'Y':
			comingSoon.countAnim += 1.0f;
			if (comingSoon.countAnim > COMINGSOON_ANIM_WAIT)
			{
				comingSoon.countAnim = 0.0f;
				// �A�j���p�^�[���̐؂�ւ�
				comingSoon.patternAnim++;
				if (comingSoon.patternAnim >= COMINGSOON_ANIM_PATTERN_NUM)
				{
					comingSoon.patternAnim = 0;
					comingSoon.useFlag = 'N';
				}
			}
			break;
		case 'N':
			break;
		default:
			break;
		}

	}

	// �ړ������ʒu�ŃA�j���[�V�����𔭐������邩�����߂�i�f�t�H���g��'N'������j
	{
		for (int i = 0; i < ANIM_MAX; i++)
		{
			stageSelectDoor[i].flameFlag = 'N';
			stageSelectAnim[i].flameFlag = 'N';
		}
	}

	// �L�[�{�[�h�œ��͏���(�g�̈ړ�)
	{
		if (GetKeyboardTrigger(DIK_RIGHT))
		{
			stageSelectFlame[0].flameSw++;	// �E�ɑI����i�߂�
			if (stageSelectFlame[0].flameSw > CHOICE_MAX - 1) stageSelectFlame[0].flameSw = CHOICE_MAX - 1;		// �v�f��Max -1�i6�j�ȏ�ɂȂ�����Ō�ɖ߂�
			PlaySound(SOUND_LABEL_SE_button);
		}
		else if (GetKeyboardTrigger(DIK_LEFT))
		{
			stageSelectFlame[0].flameSw--;	// ���ɑI����i�߂�
			if (stageSelectFlame[0].flameSw < 0) stageSelectFlame[0].flameSw = 0;	//�}�C�i�X�ɂȂ�����0�ɖ߂�
			PlaySound(SOUND_LABEL_SE_button);
		}

		stageSelect[8].pos = selectFlame[stageSelectFlame[0].flameSw];	//�g�̍��W
	}

	// �t�F�[�h��̌���ƃA�j���[�V�����ɓ����t���O�̐U�蕪��
	{
		if (stageSelect[1].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// STATUS�̈ʒu�ɂ��鎞��Enter��������A�X�e�[�^�X�I����ʂɑJ�ځi���͏�����ʂ�\���j
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}

		else if (stageSelect[2].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[0].flameFlag = 'Y';
			stageSelectAnim[0].flameFlag = 'Y';
			stageSelectAnim[0].selectNo = CHAR_SELECT_STAGE_1;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// �X�e�[�W�P�̈ʒu�ɂ��鎞��Enter��������A�Q�[���֑J��
				SetFade(FADE_OUT, MODE_GAME);
				PlaySound(SOUND_LABEL_SE_defend001);
			}
		}
		else if (stageSelect[3].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[1].flameFlag = 'Y';
			stageSelectAnim[1].flameFlag = 'Y';
			stageSelectAnim[1].selectNo = CHAR_SELECT_STAGE_2;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// �X�e�[�W�Q�̈ʒu�ɂ��鎞��Enter��������A�Q�[���֑J�ځi���͏�����ʂ�\���j
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[4].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[2].flameFlag = 'Y';
			stageSelectAnim[2].flameFlag = 'Y';
			stageSelectAnim[2].selectNo = CHAR_SELECT_STAGE_3;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// �X�e�[�W�R�̈ʒu�ɂ��鎞��Enter��������A�Q�[���֑J�ځi���͏�����ʂ�\���j
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[5].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[3].flameFlag = 'Y';
			stageSelectAnim[3].flameFlag = 'Y';
			stageSelectAnim[3].selectNo = CHAR_SELECT_STAGE_4;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// �X�e�[�W�S�̈ʒu�ɂ��鎞��Enter��������A�Q�[���֑J�ځi���͏�����ʂ�\���j
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[6].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			stageSelectDoor[4].flameFlag = 'Y';
			stageSelectAnim[4].flameFlag = 'Y';
			stageSelectAnim[4].selectNo = CHAR_SELECT_STAGE_B;

			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// �X�e�[�W�T�̈ʒu�ɂ��鎞��Enter��������A�Q�[���֑J�ځi���͏�����ʂ�\���j
				comingSoon.useFlag = 'Y';
				PlaySound(SOUND_LABEL_SE_lockon000);
			}
		}
		else if (stageSelect[7].pos == selectFlame[stageSelectFlame[0].flameSw])
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				// TITLE�ʒu�ɂ��鎞��Enter��������A�^�C�g���ɖ߂�
				SetFade(FADE_OUT, MODE_TITLE);
				PlaySound(SOUND_LABEL_SE_defend001);
			}
		}

	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawStageSelect(void)
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


	// �X�e�[�W�Z���N�g�̔w�i����`��
	{
		for (int i = 0; i < STAGESELECT_MAX; i++)
		{
			/*[1]-------------------------------------------------------------*/
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelect[i].texNo]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(vertexBuffer,
				stageSelect[i].pos.x, stageSelect[i].pos.y, stageSelect[i].w, stageSelect[i].h,
				0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// �Z���N�g�A�j���[�V������`��
	{
		for (int i = 0; i < ANIM_MAX; i++)
		{
			if (stageSelectAnim[i].flameFlag == 'Y')		// ���̊J���h�A�A�j���[�V�������g���Ă���H
			{// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelectAnim[i].texNo]);

				// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = stageSelectAnim[i].pos.x;	// �J���h�A�A�j���[�V�����̕\���ʒuX
				float py = stageSelectAnim[i].pos.y;	// �J���h�A�A�j���[�V�����̕\���ʒuY
				float pw = stageSelectAnim[i].w;		// �J���h�A�A�j���[�V�����̕\����
				float ph = stageSelectAnim[i].h;		// �J���h�A�A�j���[�V�����̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / ANIM_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / ANIM_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(stageSelectAnim[i].patternAnim % ANIM_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(stageSelectAnim[i].patternAnim / ANIM_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// �J���h�A�A�j���[�V������`��
	{
		for (int i = 0; i < DOOR_MAX; i++)
		{
			if (stageSelectDoor[i].flameFlag == 'N')		// ���̊J���h�A�A�j���[�V�������g���Ă���H
			{									// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelectDoor[i].texNo]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = stageSelectDoor[i].pos.x;	// �J���h�A�A�j���[�V�����̕\���ʒuX
				float py = stageSelectDoor[i].pos.y;	// �J���h�A�A�j���[�V�����̕\���ʒuY
				float pw = stageSelectDoor[i].w;		// �J���h�A�A�j���[�V�����̕\����
				float ph = stageSelectDoor[i].h;		// �J���h�A�A�j���[�V�����̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / DOOR_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / DOOR_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else if (stageSelectDoor[i].flameFlag == 'Y' && stageSelectDoor[i].patternAnim < DOOR_ANIM_PATTERN_NUM)		// ���̊J���h�A�A�j���[�V�������g���Ă���H
			{									// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[stageSelectDoor[i].texNo]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = stageSelectDoor[i].pos.x;	// �J���h�A�A�j���[�V�����̕\���ʒuX
				float py = stageSelectDoor[i].pos.y;	// �J���h�A�A�j���[�V�����̕\���ʒuY
				float pw = stageSelectDoor[i].w;		// �J���h�A�A�j���[�V�����̕\����
				float ph = stageSelectDoor[i].h;		// �J���h�A�A�j���[�V�����̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / DOOR_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / DOOR_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(stageSelectDoor[i].patternAnim % DOOR_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(stageSelectDoor[i].patternAnim / DOOR_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// ������ʂ�`��
	{
		if (comingSoon.useFlag == 'Y')		// ���̊J���h�A�A�j���[�V�������g���Ă���H
		{// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[comingSoon.texNo]);

			// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = comingSoon.pos.x;	// �J���h�A�A�j���[�V�����̕\���ʒuX
			float py = comingSoon.pos.y;	// �J���h�A�A�j���[�V�����̕\���ʒuY
			float pw = comingSoon.w;		// �J���h�A�A�j���[�V�����̕\����
			float ph = comingSoon.h;		// �J���h�A�A�j���[�V�����̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / COMINGSOON_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / COMINGSOON_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(comingSoon.patternAnim % COMINGSOON_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(comingSoon.patternAnim / COMINGSOON_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}