//=============================================================================
//
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct PAUSE
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�

};

struct PAUSE_MENU 
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�
	bool			selected;	// �I������Ă��邩
};

enum
{
	PAUSE_MENU_RESUME,		// ������
	PAUSE_MENU_RESTART,		// ���X�^�[�g
	PAUSE_MENU_TITLE,		// �^�C�g��
	PAUSE_MENU_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPause(void);
void UninitPause(void);
void UpdatePause(void);
void DrawPause(void);