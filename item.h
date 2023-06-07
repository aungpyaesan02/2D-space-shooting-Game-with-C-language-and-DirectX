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
#define ITEM_MAX		(10)		// �A�C�e���̐�


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum
{// �A�C�e���̏���
	E_ITEM_HP,
	E_ITEM_SHIELD,
	E_ITEM_MOVESPEED_UP,
	E_ITEM_BULLETSPEED_UP,
	E_ITEM_MAX
};

struct ITEM
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	D3DXVECTOR3		scl;			// �|���S���̊g�嗦
	D3DXVECTOR3		move;			// �ړ����x
	BOOL			display;		// true:�\��	  false:��\��
	float			w, h;			// ���ƍ���
	int				countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�
	int				tblSize;		// �o�^�����e�[�u���̃��R�[�h����
	float			moveTime;		// ���s����
	int				type;			// �A�C�e���̎��


	INTERPOLATION_DATA	*tblAdr;	// �A�j���f�[�^�̃e�[�u���擪�A�h���X


};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

ITEM *GetItem(void);



