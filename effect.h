//=============================================================================

//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define EFFECT_MAX			(64)		// �G�t�F�N�g�̍ő吔
#define EFFECT_DATA_MAX		(1)		// �G�t�F�N�g�̍ő吔

enum
{
	EFFECT_EXPLOSION,	// ����
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct EFFECT_DATA 
{
	int texNo;
	int texDivideX;
	int texDivideY;
};

struct EFFECT
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	D3DXVECTOR3		scl;			// �|���S���̊g�嗦
	BOOL			use;			// true:�g���Ă���  false:���g�p
	float			w, h;			// ���ƍ���
	int				countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	EFFECT_DATA*	data;
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

void SetEffect(int effectIndex, D3DXVECTOR3 pos, float w, float h);




