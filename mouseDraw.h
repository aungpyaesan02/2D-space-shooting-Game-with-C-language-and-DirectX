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
#define MOUSEDRAW_MAX	(100)	// �}�E�X�`���Max��

// �}�E�X�`��\����
struct MOUSEDRAW
{
	D3DXVECTOR3				pos;		// �}�E�X�`��̍��W
	BOOL					use;		// true:�g���Ă���  false:���g�p
	float					w, h;		// ���ƍ���
	int						texNo;		// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMouseDraw(void);
void UninitMouseDraw(void);
void UpdateMouseDraw(void);
void DrawMouseDraw(void);

MOUSEDRAW *GetMouseDraw(void);
void SetMouseDraw(D3DXVECTOR3 pos);


