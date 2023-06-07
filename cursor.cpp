//=============================================================================

//
//=============================================================================
#include "main.h"
#include "bg.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "title.h"
#include "mousePad.h"
#include "mouseDraw.h"
#include "mouseSkill.h"
#include "cursor.h"
#include "player.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *texturName[TEXTURE_MAX] = {

};

static BOOL		isLoad = FALSE;		// ���������s�������̃t���O

static CURSOR	cursor;				//�J�[�\���\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitCursor(void)
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


	// �ϐ��̏�����
	cursor.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	cursor.use = FALSE;


	isLoad = TRUE;				// �f�[�^�̏��������s����
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitCursor(void)
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
void UpdateCursor(void)
{
	MOUSESKILL *mouseSkill = GetMouseSkill();
	MOUSEPAD *mousePad = GetMousePad();
	MOUSEDRAW *mouseDraw = GetMouseDraw();

	// �}�E�X�̍��W���J�[�\���\���֑̂�����Ă���
	cursor.pos.x = (float)GetMousePosX();
	cursor.pos.y = (float)GetMousePosY();

	// �J�[�\�����N���b�N�ŕM�̃h���[����
	if (IsMouseLeftPressed())
	{// Enter��������A�X�e�[�W��؂�ւ���
		D3DXVECTOR3 pos = D3DXVECTOR3((float)GetMousePosX() , (float)GetMousePosY(), 0.0f);
		SetMouseDraw(pos);		// �P����

	}

	// �J�[�\���E�N���b�N�ŕM�̃h���[�Ə����ɂ���ăX�L������
	
	MouseSkillSet();	// �� ���̊֐����ɏ����ڍ׋L��

#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("Cursor X:%f \n", cursor.pos.x);
	PrintDebugProc("Cursor Y:%f \n", cursor.pos.y);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCursor(void)
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
}


//=============================================================================
// �e�N�X�`���̍��W��ɃJ�[�\�������邩�擾����֐�
//=============================================================================
BOOL CheckCursorPos(float x, float y, float w, float h)
{
	// �e�N�X�`���i�����̕����j�̍��W��ɃJ�[�\�������邩?
	if (   x <= (float)GetMousePosX()
		&& y <= (float)GetMousePosY()
		&& w >= (float)GetMousePosX()
		&& h >= (float)GetMousePosY())
	{
		// ��������TRUE����
		cursor.use = TRUE;
	}
	else
	{
		// �Ȃ�����FALSE����
		return cursor.use = FALSE;
	}

	// ��������l��Ԃ�
	return cursor.use;

}


//=============================================================================
// �}�E�X�X�L���̃Z�b�g
//=============================================================================
void MouseSkillSet(void)
{
	PLAYER *player = GetPlayer();
	MOUSESKILL *mouseSkill = GetMouseSkill();
	MOUSEPAD *mousePad = GetMousePad();
	MOUSEDRAW *mouseDraw = GetMouseDraw();player->pos;

	// �����ɂ���ă}�E�X�E�N���b�N����������A�}�E�X�X�L������
	// �}�E�X�X�L���ƕM�̃h���[�L�����Z��
	for (int i = 0; MOUSEDRAW_MAX > i; i++)
	{
		for (int j = 0; MOUSEPAD_MAX > j; j++)
		{
			/* �����Ƀ}�E�X����p�p�b�h�̓����蔻���ԂƉE�N���b�N�ɉ������X�L����ǉ����Ă���*/


			/* SET_SKILL_RIGHTSHOT */
			/*
					�^0�_
					�_ �^2�_
					   �_ �^5�_
					   �^7�_ �^
					�^8�_ �^
					�_ �^
			*/
			if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == FALSE &&
				mousePad[2].use == TRUE  && mousePad[3].use == FALSE &&
				mousePad[4].use == FALSE && mousePad[5].use == TRUE  &&
				mousePad[6].use == FALSE && mousePad[7].use == TRUE  &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos;	// �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_RIGHTSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE;	// ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}
			}


			/* SET_SKILL_LEFTSHOT */
			/*
					�^0�_
				 �^1�_ �^
			  �^3�_ �^
			  �_ �^6�_
				 �_ �^8�_
					�_ �^
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == TRUE  &&
				mousePad[2].use == FALSE && mousePad[3].use == TRUE  &&
				mousePad[4].use == FALSE && mousePad[5].use == FALSE &&
				mousePad[6].use == TRUE  && mousePad[7].use == FALSE &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_LEFTSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}
			}


			/* SET_SKILL_UPSHOT */
			/*
					�^0�_
				 �^1�_ �^2�_
			  �^3�_ �^ �_ �^5�_
			  �_ �^		  �_ �^


			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == TRUE  &&
				mousePad[2].use == TRUE  && mousePad[3].use == TRUE  &&
				mousePad[4].use == FALSE && mousePad[5].use == TRUE  &&
				mousePad[6].use == FALSE && mousePad[7].use == FALSE &&
				mousePad[8].use == FALSE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_UPSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}
			}


			/* SET_SKILL_DOWNSHOT */
			/*


			  �^3�_		  �^5�_
			  �_ �^6�_ �^7�_ �^
				 �_ �^8�_ �^
					�_ �^
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == FALSE && mousePad[1].use == FALSE &&
				mousePad[2].use == FALSE && mousePad[3].use == TRUE  &&
				mousePad[4].use == FALSE && mousePad[5].use == TRUE  &&
				mousePad[6].use == TRUE  && mousePad[7].use == TRUE  &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_DOWNSHOT;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}
			}


			/* SET_SKILL_CROSS */
			/*

				 �^1�_ �^2�_
				 �_ �^4�_ �^
				 �^6�_ �^7�_
				 �_ �^ �_ �^

			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == FALSE && mousePad[1].use == TRUE  &&
				mousePad[2].use == TRUE  && mousePad[3].use == FALSE &&
				mousePad[4].use == TRUE  && mousePad[5].use == FALSE &&
				mousePad[6].use == TRUE  && mousePad[7].use == TRUE  &&
				mousePad[8].use == FALSE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_CROSS;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j


					// SE
					PlaySound(SOUND_LABEL_SE_mousepop);
				}
			}


			/* SET_SKILL_STUN */
			/*
					�^0�_
				 �^1�_ �^
				 �_ �^4�_
					�_ �^7�_
					�^8�_ �^
					�_ �^
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE  && mousePad[1].use == TRUE  &&
				mousePad[2].use == FALSE && mousePad[3].use == FALSE &&
				mousePad[4].use == TRUE  && mousePad[5].use == FALSE &&
				mousePad[6].use == FALSE && mousePad[7].use == TRUE  &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_STUN;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}
			}


			/* SET_SKILL_HOMING */
			/*
					�^0�_
				 �^1�_ �^2�_
				 �_ �^4�_ �^
				 �^6�_ �^7�_
				 �_ �^ �_ �^

			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE && mousePad[1].use == TRUE  &&
				mousePad[2].use == TRUE && mousePad[3].use == FALSE &&
				mousePad[4].use == TRUE && mousePad[5].use == FALSE &&
				mousePad[6].use == TRUE && mousePad[7].use == TRUE  &&
				mousePad[8].use == FALSE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_HOMING;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}

				// SE

			}


			/* SET_SKILL_VORTEX */
			/*
					�^0�_
				 �^1�_ �^2�_
			  �^3�_ �^4�_ �^5�_
			  �_ �^6�_ �^7�_ �^
				 �_ �^8�_ �^
					�_ �^
			*/
			else if (mouseDraw[i].use == TRUE &&
				mousePad[0].use == TRUE && mousePad[1].use == TRUE &&
				mousePad[2].use == TRUE && mousePad[3].use == TRUE &&
				mousePad[4].use == TRUE && mousePad[5].use == TRUE &&
				mousePad[6].use == TRUE && mousePad[7].use == TRUE &&
				mousePad[8].use == TRUE)
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L������
					D3DXVECTOR3 pos = player->pos; // �v���C���[�̃|�W�V�����������ɂ���B
					int skill = SET_SKILL_VORTEX;
					SetMouseSkill(pos, skill);
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j

					// SE
					PlaySound(SOUND_LABEL_SE_mousepop);

				}
			}


			/* SET_SKILL_�ǉ��p */
			/*
					�^0�_
				 �^1�_ �^2�_
			  �^3�_ �^4�_ �^5�_
			  �_ �^6�_ �^7�_ �^
				 �_ �^8�_ �^
					�_ �^
			*/


			else
			{
				if (IsMouseRightTriggered())
				{// ���̏����Ń}�E�X�E�N���b�N����������A�}�E�X�X�L�������͂��Ȃ�
					mouseDraw[i].use = FALSE;	// ���g�p�i�`�悳��Ă��Ȃ��}�E�X�O�Ղɂ���j
					mousePad[j].use = FALSE; // ���g�p�i�g�p����Ă��Ȃ��}�E�X����p�p�b�h��Ԃɂ���j
				}
			}
		}
	}
}