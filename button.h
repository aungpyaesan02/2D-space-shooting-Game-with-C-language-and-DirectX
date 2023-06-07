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

struct BUTTON
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	D3DXCOLOR		color;
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�
	bool			use;
};

enum
{
	START_BUTTON,
	EXIT_BUTTON,
	PLAYER_BUTTON,
	TITLE_LOGO,
	BUTTON_MAX
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitButton(void);
void UninitButton(void);
void UpdateButton(void);
void DrawButton(void);

void GameStartAnimation(void);
void SelectMenu(void);


