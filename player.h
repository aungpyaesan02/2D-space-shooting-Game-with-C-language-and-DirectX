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
#define PLAYER_MAX					(1)		// �v���C���[��Max�l��

#define DAMAGE						(1)		// �󂯂��U���̒l


enum
{
	CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,
	CHAR_DIR_RIGHT,
	CHAR_DIR_UP,
	CHAR_DIR_MAX
};

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct PLAYER
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	BOOL			use;			// true:�g���Ă���  false:���g�p
	float			w, h;			// ���ƍ���
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�
	int				hp;				// �q�b�g�|�C���g
	D3DXVECTOR3		move;			// �ړ����x

	int				dir;			// �����i�ڍׂ�player.h��enum���Q�Ɓj
	BOOL			moving;			// Key������Ă��邩�t���O


	float			shieldTime;		// �V�[���h��������
	float			speedUpTime;	// ���x��������
	float			bulletSpeedUpTime;	// ���x��������
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void SetPlayerHP(int hp);
void SetPlayerShield(float shieldTime);
void SetPlayerMoveSpeed(float moveSpeedUp);
void SetPlayerBulletSpeed(float bulletSpeedUp);



