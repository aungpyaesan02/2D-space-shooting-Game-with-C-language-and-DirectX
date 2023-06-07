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
#define STAGESELECT_MAX		(9)				// �X�e�[�W�Z���N�g��Max��
#define ANIM_MAX			(5)				// �Z���N�g�A�j���[�V������Max��
#define DOOR_MAX			(5)				// �h�A�A�j���[�V������Max��

enum
{
	CHAR_SELECT_STAGE_1,	// �X�e�[�W�P
	CHAR_SELECT_STAGE_2,	// �X�e�[�W�Q
	CHAR_SELECT_STAGE_3,	// �X�e�[�W�R
	CHAR_SELECT_STAGE_4,	// �X�e�[�W�S
	CHAR_SELECT_STAGE_B, 	// �X�e�[�W�{�X
	CHAR_SELECT_STAGE_NONE 	// �X�e�[�W�m�[�Z���N�g

};

// �X�e�[�W�Z���N�g�̍\����
struct STAGESELECT
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
};

// �Z���N�g�p�t���[���\����
struct STAGESELECT_FLAME
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	int				flameSw;		// �t���[���؂�ւ��p
};

// �Z���N�g�A�j���[�V�����p�\����
struct STAGESELECT_ANIM
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	BOOL			selectNo;		// �ǂ̃X�e�[�W���Z���N�g���Ă��邩�ŃA�j���[�V������؂�ւ���ienum�Q�Ɓj
	char			flameFlag;		// �I��p�t���[�������邩�Ȃ����̃t���O�iY:���� N:�Ȃ��j
};

// �J���h�A�A�j���[�V�����p�\����
struct STAGESELECT_DOOR
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	char			flameFlag;		// �I��p�t���[�������邩�Ȃ����̃t���O�iY:���� N:�Ȃ��j
};

// ������ʗp�\����
struct COMINGSOON
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	char			useFlag;		// �I��p�t���[�������邩�Ȃ����̃t���O�iY:���� N:�Ȃ��j
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitStageSelect(void);
void UninitStageSelect(void);
void UpdateStageSelect(void);
void DrawStageSelect(void);
