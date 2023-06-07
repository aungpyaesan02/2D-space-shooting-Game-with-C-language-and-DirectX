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
#define	MOUSESKILL_MAX			(1)			// �}�E�X�X�L����Max��
#define	MOUSESKILL_DW			(100.0f)	// �\���T�C�Y�i���j
#define	MOUSESKILL_DH			(100.0f)	// �\���T�C�Y�i�c�j

// �}�E�X�X�L���\����
struct MOUSESKILL
{
	D3DXVECTOR3		pos;			// �}�E�X�X�L���̍��W
	D3DXVECTOR3		rot;			// �}�E�X�X�L���̉�]
	BOOL			use;			// ���̃}�E�X�X�L�����g���Ă��邩
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
	int				countAnim;		// �A�j���[�V�����J�E���g
	//int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				skillFlag;		// �}�E�X�X�L���I��p
	int				skillSwitch;	// �}�E�X�X�L���X�C�b�`�p
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMouseSkill(void);
void UninitMouseSkill(void);
void UpdateMouseSkill(void);
void DrawMouseSkill(void);

MOUSESKILL *GetMouseSkill(void);
void SetMouseSkill(D3DXVECTOR3 pos, int skill);
void MouseSkillSelect(void);
