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
#define SCORE_MAX			(99999)			// �X�R�A�̍ő�l
#define SCORE_DIGIT			(5)				// ����
#define HIGHSCORE_MAX		(10)			// �Z�[�u����n�C�X�R�A�̐�

#define	SCORE_DX			(540.0f)		// �\���ʒu
#define	SCORE_DY			(20.0f)			// 
#define	SCORE_DW			(32.0f)			// 
#define	SCORE_DH			(64.0f)			// 

#define	HIGHSCORE_DX			(400.0f)	// �\���ʒu
#define	HIGHSCORE_DY			(100.0f)	// 
#define	HIGHSCORE_DW			(32.0f)		// 
#define	HIGHSCORE_DH			(64.0f)		// 

#define HIGHSCORE_LEN_X			(360)	// �n�C�X�R�A����ׂ�Ԋu
#define HIGHSCORE_LEN_Y			(60)

#define ENEMY_DESTORY_SCORE		(120)	// �G�l�~�[��|�������̃X�R�A���Z�l

struct SCORE
{
	D3DXVECTOR3		pos;					// �|���S���̍��W
	float			w, h;					// ���ƍ���
	int				texNo;					// �g�p���Ă���e�N�X�`���ԍ�
	int				score;					// ���_
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

int GetScore(void);
void AddScore(int add);
void SetScore(int score);


