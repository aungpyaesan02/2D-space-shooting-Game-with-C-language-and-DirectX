//=============================================================================
//
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "enemy.h"
#include "bullet.h"
#include "playerBullet.h"
#include "collision.h"
#include "sound.h"
#include "fade.h"
#include "result.h"
#include "file.h"
#include "vibration.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(80)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(80)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define PLAYER_SPEED				(7.0f)	// �v���C���[�̕W�����x

#define HP_MAX						(10)	// HP�̍ő�l
#define SPEED_UP					(4.0f)		// �X�s�[�h�A�b�v�A�C�e���擾���̑��x



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/PLAYER/ship_001.png",
};


static BOOL		isLoad = FALSE;			// ���������s�������̃t���O
static PLAYER	player[PLAYER_MAX];	// �v���C���[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			TexturName[i],
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


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i].use = TRUE;
		player[i].pos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);	// ���S�_����\��
		player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		player[i].w   = TEXTURE_WIDTH;
		player[i].h   = TEXTURE_HEIGHT;
		player[i].texNo = 0;
		player[i].hp = HP_MAX;
		player[i].shieldTime = 0.0f;
		player[i].speedUpTime = 0.0f;
		player[i].bulletSpeedUpTime = 0.0f;

		player[i].hp = HP_MAX;

		player[i].countAnim = 0;
		player[i].patternAnim = 0;

		player[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// �ړ���

		player[i].dir = CHAR_DIR_DOWN;						// ������
		player[i].moving = FALSE;							// Key������Ă��邩�t���O
		player[i].patternAnim = player[i].dir * TEXTURE_PATTERN_DIVIDE_X;
	}

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (player[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 posOld = player[i].pos;


			// �A�j���[�V���� 
			if (player[i].moving == TRUE)
			{
				player[i].countAnim += 1.0f;
				if (player[i].countAnim > ANIM_WAIT)
				{
					player[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					player[i].patternAnim = (player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}

			// �L�[���͂ňړ� 
			{
				// �ړ����x
				if (player[0].speedUpTime > 0.0f)
				{
					player[i].move.x = PLAYER_SPEED + SPEED_UP;

				}
				else
				{
					player[i].move.x = PLAYER_SPEED;
				}

				float speed = player[i].move.x;

				player[i].moving = FALSE;


				if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
				{
					player[i].pos.y += speed;
					player[i].dir = CHAR_DIR_DOWN;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 3.14f);
					player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
				{
					player[i].pos.y -= speed;
					player[i].dir = CHAR_DIR_UP;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					player[i].moving = TRUE;
				}

				if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
				{
					player[i].pos.x += speed;
					player[i].dir = CHAR_DIR_RIGHT;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 1.57f);
					player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
				{
					player[i].pos.x -= speed;
					player[i].dir = CHAR_DIR_LEFT;
					player[i].rot = D3DXVECTOR3(0.0f, 0.0f, -1.57f);
					player[i].moving = TRUE;
				}

				// �Q�[���p�b�h�łňړ�����
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					player[i].pos.y += speed;
					player[i].dir = CHAR_DIR_DOWN;
					player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_UP))
				{
					player[i].pos.y -= speed;
					player[i].dir = CHAR_DIR_UP;
					player[i].moving = TRUE;
				}

				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					player[i].pos.x += speed;
					player[i].dir = CHAR_DIR_RIGHT;
					player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					player[i].pos.x -= speed;
					player[i].dir = CHAR_DIR_LEFT;
					player[i].moving = TRUE;
				}

				// �e����
				//if (GetKeyboardPress(DIK_SPACE) || IsButtonPressed(0, BUTTON_R))
				//{

				// �v���C���[�̌����Ńo���b�g�̕�����ς���
				switch (player[i].dir)
				{
				case CHAR_DIR_DOWN:// ������
					SetPlayerBullet(player[i].pos, CHAR_DIR_DOWN);
					break;
				case CHAR_DIR_LEFT:// ������
					SetPlayerBullet(player[i].pos, CHAR_DIR_LEFT);
					break;
				case CHAR_DIR_RIGHT:// �E����
					SetPlayerBullet(player[i].pos, CHAR_DIR_RIGHT);
					break;
				case CHAR_DIR_UP:	// �����
					SetPlayerBullet(player[i].pos, CHAR_DIR_UP);
					break;
				default:
					break;
				}

				//}


				// MAP�O�`�F�b�N
				BG *bg = GetBG();

				if (player[i].pos.x - TEXTURE_WIDTH / 2 < 0.0f)
				{
					player[i].pos.x = 0.0f + TEXTURE_WIDTH / 2;
				}

				if (player[i].pos.x + TEXTURE_WIDTH / 2 > bg->w)
				{
					player[i].pos.x = bg->w - TEXTURE_WIDTH / 2;
				}

				if (player[i].pos.y - TEXTURE_HEIGHT / 2 < 0.0f)
				{
					player[i].pos.y = 0.0f + TEXTURE_HEIGHT / 2;
				}

				if (player[i].pos.y + TEXTURE_HEIGHT / 2 > bg->h)
				{
					player[i].pos.y = bg->h - TEXTURE_HEIGHT / 2;
				}

				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				bg->pos.x = player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				{
					ENEMY *enemy = GetEnemy();
					// �V�[���h���������Ă���H
					if (player[0].shieldTime <= 0.0f)
					{// No
						// �G�l�~�[�̐��������蔻����s��
						for (int j = 0; j < ENEMY_MAX; j++)
						{
							// �����Ă�G�l�~�[�Ɠ����蔻�������
							if (enemy[j].use == TRUE)
							{

								BOOL ans = CollisionBB(player[i].pos, player[i].w, player[i].h,
									enemy[j].pos, enemy[j].w, enemy[j].h);

								// �������Ă���H
								if (ans == TRUE)
								{
									// �����������̏���
									player[0].hp -= DAMAGE;
									SetVibration();
									DestroyEnemy(j);
									if (player[0].hp <= 0)
									{
										player[i].use = FALSE;
										SetFade(FADE_OUT, MODE_RESULT);
									}

								}
							}
						}
					}
				}
			}
		}


		// �V�[���h�L�����Ԃ̌v�Z
		player[0].shieldTime -= 0.1f;
		if (player[0].shieldTime < 0.0f)
		{
			player[0].shieldTime = 0.0f;
		}		

		// �v���C���[�̃X�s�[�h�A�b�v���Ԃ̌v�Z
		player[0].speedUpTime -= 0.1f;
		if (player[0].speedUpTime < 0.0f)
		{
			player[0].speedUpTime = 0.0f;
		}

		// �e�̃X�s�[�h�A�b�v���Ԃ̌v�Z
		player[0].bulletSpeedUpTime -= 0.1f;
		if (player[0].bulletSpeedUpTime < 0.0f)
		{
			player[0].bulletSpeedUpTime = 0.0f;
		}


#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Player No%d  X:%f Y:%f\n", i, player[i].pos.x, player[i].pos.y);
		PrintDebugProc("hp = %d\n", player[i].hp);
		PrintDebugProc("ShieldTime = %f\n", player[0].shieldTime);
		PrintDebugProc("SpeedUpTime = %f\n", player[0].speedUpTime);

		// �t�F�[�h�p�̃e�X�g
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter��������A�X�e�[�W��؂�ւ���
			// ���U���g�ɃX�R�A���Z�b�g����
			SetResult(GetScore());
			SaveHighscore();
			SetFade(FADE_OUT, MODE_RESULT);	// ���U���g��ʂ�

			SetFade(FADE_OUT, MODE_RESULT);
		}


#endif

	}
	
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = player[i].w;		// �v���C���[�̕\����
			float ph = player[i].h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				player[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �v���C���[�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER *GetPlayer(void)
{

	return &player[0];
}

//=============================================================================
// �v���C���[�񕜂̊֐�
//=============================================================================
void SetPlayerHP(int hp)
{
	player[0].hp += hp;
	if (player[0].hp > 10)
	{
		player[0].hp = 10;
	}
}

//=============================================================================
// �v���C���[�V�[���h�������Ԃ�ݒ肷��֐�
//=============================================================================
void SetPlayerShield(float shieldTime)
{
	player[0].shieldTime = shieldTime;
}

//=============================================================================
// �v���C���[�ړ����x���A�b�v���鎞�Ԃ�ݒ肷��֐�
//=============================================================================
void SetPlayerMoveSpeed(float moveSpeedUp)
{
	player[0].speedUpTime = moveSpeedUp;
}

//=============================================================================
// �v���C���[�̒e���x���A�b�v���鎞�Ԃ�ݒ肷��֐�
//=============================================================================
void SetPlayerBulletSpeed(float bulletSpeedUp)
{
	player[0].bulletSpeedUpTime = bulletSpeedUp;
}