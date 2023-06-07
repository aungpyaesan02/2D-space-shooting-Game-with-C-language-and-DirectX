//=============================================================================
//
//
//=============================================================================
#include "item.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "player.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(35)			// �A�C�e���T�C�Y
#define TEXTURE_HEIGHT				(35)			// 
#define TEXTURE_MAX					(E_ITEM_MAX)	// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)				// �A�j���[�V�����̐؂�ւ��Wait�l

#define SHIELD_TIME					(5.0f * 6.0f)	// �V�[���h�L������	�ix�b �� (xf * 6.0f)�j
#define HP_RECOVERY					(1)				// �񕜗�
#define MOVESPEED_UP_TIME			(3.0f * 6.0f)	// �ړ���������		�ix�b �� (xf * 6.0f)�j
#define BULLETSPEED_UP_TIME			(10.0f * 6.0f)	// �e��������		�ix�b �� (xf * 6.0f)�j


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *texturName[TEXTURE_MAX] = {
	"data/TEXTURE/ITEM/hp001.png",
	"data/TEXTURE/ITEM/shield.png",
	"data/TEXTURE/ITEM/speed_up.png",
	"data/TEXTURE/ITEM/bullet_speed_up.png",
};


static BOOL		isLoad = FALSE;			// ���������s�������̃t���O
static ITEM		item[ITEM_MAX];			// �A�C�e���\����

static float itemPopTime;						// �A�C�e���|�b�v����


// ��Ԃł̈ړ��e�[�u��
static INTERPOLATION_DATA moveTbl0[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(700.0f, 700.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(750.0f, 700.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(700.0f, 700.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};
static INTERPOLATION_DATA moveTbl1[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(300.0f, 300.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(150.0f, 300.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(300.0f, 300.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};
static INTERPOLATION_DATA moveTbl2[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(1600.0f, 1200.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1300.0f, 1200.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1600.0f, 1200.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};
static INTERPOLATION_DATA moveTbl3[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(1500.0f, 500.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1200.0f, 500.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },
	{ D3DXVECTOR3(1500.0f, 500.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f), 60 * 1 },

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			texturName[i],
			NULL,
			NULL,
			&texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �A�C�e���\���̂̏�����
	for (int i = 0; i < ITEM_MAX; i++)
	{
		item[i].display = FALSE;
		item[i].pos     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		item[i].rot     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		item[i].scl     = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		item[i].w       = TEXTURE_WIDTH;
		item[i].h       = TEXTURE_HEIGHT;
		item[i].move    = D3DXVECTOR3(4.0f, 0.0f, 0.0f);

		item[i].texNo = i;

		item[i].countAnim = 0;
		item[i].patternAnim = 0;

		item[i].type = 0;

		item[i].moveTime = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		item[i].tblAdr   = NULL;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		item[i].tblSize  = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	}



	/*�A�C�e���ɐ��`��Ԃ̃f�[�^��o�^*/
	{
		// 0�Ԗڂ̃A�C�e����moveTbl0��o�^
		item[E_ITEM_HP].moveTime = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		item[E_ITEM_HP].tblAdr   = moveTbl0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		item[E_ITEM_HP].tblSize  = sizeof(moveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// 1�Ԗڂ̃A�C�e����moveTbl1��o�^
		item[E_ITEM_SHIELD].moveTime = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		item[E_ITEM_SHIELD].tblAdr = moveTbl1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		item[E_ITEM_SHIELD].tblSize = sizeof(moveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// 2�Ԗڂ̃A�C�e����moveTbl2��o�^
		item[E_ITEM_MOVESPEED_UP].moveTime = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		item[E_ITEM_MOVESPEED_UP].tblAdr = moveTbl2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		item[E_ITEM_MOVESPEED_UP].tblSize = sizeof(moveTbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// 3�Ԗڂ̃A�C�e����moveTbl3��o�^
		item[E_ITEM_BULLETSPEED_UP].moveTime = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		item[E_ITEM_BULLETSPEED_UP].tblAdr = moveTbl3;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		item[E_ITEM_BULLETSPEED_UP].tblSize = sizeof(moveTbl3) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	}




	// �A�C�e���|�b�v����
	itemPopTime = 0;


	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitItem(void)
{
	if (isLoad == FALSE) return;

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (texture[i])
		{
			texture[i]->Release();
			texture[i] = NULL;
		}
	}

	isLoad = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void)
{
	// ���Ԍv��
	itemPopTime += 1.0f;
	float minutes = itemPopTime / 3600.0f;	// 1/3600�t���[���֕ϊ�

	// ����̎��ԂɃ����_���ŃA�C�e�����o��
	if (minutes >= 0.2f)
	{
		for (int i = 0; i < TEXTURE_MAX; i++)
		{
			//�A�C�e�������łɑ��݂��Ă��邩�`�F�b�N����
			if (item[i].display == TRUE)
				continue;


			int itemPop = rand() % TEXTURE_MAX;
			BOOL checkPop = FALSE;			// �|�b�v�������m�F

			// ���݂��Ă��Ȃ��H
			if (item[itemPop].display == FALSE)
			{// Yes
				item[itemPop].display = TRUE;
				checkPop = TRUE;
			}

			// �A�C�e���|�b�v�����玞�Ԃ�������
			if (checkPop == TRUE)
			{
				itemPopTime = 0.0f;
				minutes = 0.0f;
				checkPop = FALSE;
			}
		}
	}


	for (int i = 0; i < ITEM_MAX; i++)
	{

		// ���̃A�C�e���͖��g�p���\�����H
		if (item[i].display == TRUE)
		{// Yes
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 posOld = item[i].pos;

			// �A�j���[�V����  
			item[i].countAnim++;
			if ((item[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				item[i].patternAnim = (item[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �ړ����� 
			if (item[i].tblAdr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)item[i].moveTime;
				float	time = item[i].moveTime - index;
				int		size = item[i].tblSize;

				float dt = 1.0f / item[i].tblAdr[index].frame;	// 1�t���[���Ői�߂鎞��
				item[i].moveTime += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					item[i].moveTime = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				item[i].pos = (1 - time) * item[i].tblAdr[index].pos + time * item[i].tblAdr[index + 1].pos;

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				item[i].rot = (1 - time) * item[i].tblAdr[index].rot + time * item[i].tblAdr[index + 1].rot;

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				item[i].scl = (1 - time) * item[i].tblAdr[index].scl + time * item[i].tblAdr[index + 1].scl;

			}
			else
			{
				item[i].pos += item[i].move;
			}

			// MAP�O����
			{
				BG *bg = GetBG();

				if ((item[i].pos.x < -item[i].w) || (item[i].pos.x > bg->w + item[i].w) ||
					(item[i].pos.y < -item[i].h) || (item[i].pos.y > bg->w + item[i].h))
				{
					item[i].display = FALSE;
				}

			}






			// �v���C���[�����擾
			PLAYER *player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				//�G�̗L���t���O���`�F�b�N����
				if (player[j].use == FALSE)
					continue;

				BOOL ans = CollisionBB(player[j].pos, player[j].w, player[j].h,
					item[i].pos, item[i].w, item[i].h);
				if (ans == TRUE)
				{
					switch (i)
					{
					case E_ITEM_HP:
						SetPlayerHP(HP_RECOVERY);
						break;
					case E_ITEM_SHIELD:
						SetPlayerShield(SHIELD_TIME);
						break;
					case E_ITEM_MOVESPEED_UP:
						SetPlayerMoveSpeed(MOVESPEED_UP_TIME);
						break;
					case E_ITEM_BULLETSPEED_UP:
						SetPlayerBulletSpeed(BULLETSPEED_UP_TIME);
						break;
					}

					item[i].display = FALSE;

				}
			}

		}

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Item No%d  X:%f Y:%f\n", i, item[i].pos.x, item[i].pos.y);
	}
	PrintDebugProc("seconds = %f\n", itemPopTime);
	PrintDebugProc("minutes = %f\n", minutes);

	PrintDebugProc("item0 = %d\n", item[E_ITEM_HP].display);
	PrintDebugProc("item1 = %d\n", item[E_ITEM_SHIELD].display);
	PrintDebugProc("item2 = %d\n", item[E_ITEM_MOVESPEED_UP].display);
	PrintDebugProc("item3 = %d\n", item[E_ITEM_BULLETSPEED_UP].display);
#endif
	}
}
	




//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG *bg = GetBG();

	for (int i = 0; i < ITEM_MAX; i++)
	{
		// ���̃A�C�e���͖��g�p���\�����H
		if (item[i].display == TRUE)
		{// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[item[i].texNo]);

			//�A�C�e���̈ʒu��e�N�X�`���[���W�𔽉f
			float px = item[i].pos.x - bg->pos.x;	// �A�C�e���̕\���ʒuX
			float py = item[i].pos.y - bg->pos.y;	// �A�C�e���̕\���ʒuY
			float pw = item[i].w;					// �A�C�e���̕\����
			float ph = item[i].h;					// �A�C�e���̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(item[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(item[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				item[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}
}



//=============================================================================
// �G�l�~�[�\���̂̐擪�A�h���X���擾
//=============================================================================
ITEM *GetItem(void)
{

	return &item[0];
}




