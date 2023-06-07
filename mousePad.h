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
#define	MOUSEPAD_MAX		(9)			// �}�E�X����p�p�b�h��Max��
#define	MOUSEPAD_DW			(40.0f)		// �\���T�C�Y�i���j
#define	MOUSEPAD_DH			(40.0f)		// �\���T�C�Y�i�c�j
#define	MOUSEPAD_DX			(850.0f)	// �\���ʒu�i���j
#define	MOUSEPAD_DY			(420.0f)	// �\���ʒu�i�c�j
#define	MOUSEPAD_DX1		(70.0f)		// �ʌz�u�p�ʒu1�i���j
#define	MOUSEPAD_DY1		(70.0f)		// �ʌz�u�p�ʒu1�i�c�j
#define	MOUSEPAD_DX2		(35.0f)		// �ʌz�u�p�ʒu2�i���j
#define	MOUSEPAD_DY2		(35.0f)		// �ʌz�u�p�ʒu2�i�c�j

// �}�E�X����p�p�b�h�\����
struct MOUSEPAD
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	D3DXVECTOR3		rot;		// �|���S���̍��W
	BOOL			use;		// �g�p���g�p
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMousePad(void);
void UninitMousePad(void);
void UpdateMousePad(void);
void DrawMousePad(void);

MOUSEPAD *GetMousePad(void);

