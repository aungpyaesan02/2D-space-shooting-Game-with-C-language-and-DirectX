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
#define ENEMY_MAX		(256)		// �G�l�~�[��Max�l��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	D3DXVECTOR3		scl;			// �|���S���̊g�嗦
	BOOL			use;			// true:�g���Ă���  false:���g�p
	float			w, h;			// ���ƍ���
	int				countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�
	int				countBullet;	// �e�����p�̃J�E���g
	int				countMax;		// �������Ԋu
	
	D3DXVECTOR3		move;			// �ړ����x

	INTERPOLATION_DATA	*tblAdr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tblSize;			// �o�^�����e�[�u���̃��R�[�h����
	float				moveTime;			// ���s����

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
void SetEnemy(void);
void DestroyEnemy(int i);