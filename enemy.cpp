//=============================================================================
//

//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "player.h"
#include "collision.h"
#include "bullet.h"
#include "effect.h"
#include "score.h"
#include "sound.h"
#include "Vibration.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// �G�l�~�[�T�C�Y
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(5)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X		(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y		(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM				(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define ENEMY_SPAWN_WAIT_DEFAULT		(200)	// �G�l�~�[�̃X�|�[���Ԋu
#define ENEMY_SPAWN_NUM				(3)		// �P��̃X�|�[���Ō����G�̐�
#define ENEMY_SPAWN_RANGE			(300)	// �G�l�~�[�̃X�|�[���ʒu

#define ENEMY_CHANGE_MOVE_PROB		( 1)		// �G�l�~�[���ړ�������ς���m���i%�j

#define ENEMY_MOVE_SPD				(2.0f)	// �G�l�~�[�̈ړ����x

#define ENEMY_ATK_RANGE				(500)	// �G�l�~�[�̍U���͈�	


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/ENEMY/enemy_001.png",
	"data/TEXTURE/ENEMY/enemy_002.png",
	"data/TEXTURE/ENEMY/enemy_003.png",
	"data/TEXTURE/ENEMY/enemy_004.png",
	"data/TEXTURE/ENEMY/enemy_005.png",
};


static BOOL		isLoad = FALSE;			// ���������s�������̃t���O
static ENEMY		enemy[ENEMY_MAX];		// �G�l�~�[�\����

// ��Ԃł̈ړ��e�[�u��
static INTERPOLATION_DATA moveTbl[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3(600.0f, 400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(300.0f, 400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(300.0f, 100.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(600.0f, 100.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ D3DXVECTOR3(600.0f, 400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 2 },

};

static INTERPOLATION_DATA moveTbl2[] = {	// pos, rot, scl, frame
	{ D3DXVECTOR3( 800.0f, 800.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 3 },
	{ D3DXVECTOR3(1400.0f, 800.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ D3DXVECTOR3(1400.0f,1400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ D3DXVECTOR3( 800.0f,1400.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ D3DXVECTOR3( 800.0f, 800.0f,  0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60 * 1 },

};

static float spawnTimer = 0.0f;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
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


	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		enemy[i].use = FALSE;
		enemy[i].pos = D3DXVECTOR3(50.0f + i*150, 100.0f, 0.0f);
		enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemy[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		enemy[i].w   = TEXTURE_WIDTH;
		enemy[i].h   = TEXTURE_HEIGHT;

		enemy[i].texNo = rand()% TEXTURE_MAX;

		enemy[i].countAnim = 0;
		enemy[i].patternAnim = 0;

		enemy[i].move = D3DXVECTOR3(ENEMY_MOVE_SPD, 0.0f, 0.0f);

		enemy[i].moveTime = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		enemy[i].tblAdr = NULL;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		enemy[i].tblSize = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		enemy[i].countBullet = 0;
		enemy[i].countMax = 300;
	}

	// ���`��Ԃœ������G�l�~�[�Ƀf�[�^�o�^
	enemy[0].use = TRUE;
	enemy[0].moveTime = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	enemy[0].tblAdr = moveTbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	enemy[0].tblSize = sizeof(moveTbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	enemy[1].use = TRUE;
	enemy[1].moveTime = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	enemy[1].tblAdr = moveTbl2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	enemy[1].tblSize = sizeof(moveTbl2) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	spawnTimer = 0.0f;

	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	// �G�l�~�[�̃X�|�[������
	spawnTimer++;
	if (spawnTimer > ENEMY_SPAWN_WAIT_DEFAULT)
	{
		spawnTimer = 0.0f;
		
		int num = rand() % ENEMY_SPAWN_NUM;
		for (int i = 0; i < num; i++)
		{
			SetEnemy();
		}
	}

	PLAYER *player = GetPlayer();
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == TRUE)	// ���̃G�l�~�[���g���Ă���H
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = enemy[i].pos;

			// �A�j���[�V����  
			enemy[i].countAnim++;
			if ((enemy[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				enemy[i].patternAnim = (enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			int diffX = (int)(player[0].pos.x - enemy[i].pos.x);
			int diffY = (int)(player[0].pos.y - enemy[i].pos.y);
			if (diffX + diffY < ENEMY_ATK_RANGE)
			{
				enemy[i].countBullet++;
			}
			
			if (enemy[i].countBullet >= enemy[i].countMax)
			{
				enemy[i].countBullet = 0;
				SetBullet(enemy[i].pos);

				// ���`��ԈȊO�̃G�l�~�[�͈ړ�������ς���
				if (enemy[i].tblAdr == NULL)
				{
					float randomSpd = (float)(rand() % (int)(ENEMY_MOVE_SPD * 0.5f) + (int)(ENEMY_MOVE_SPD * 0.5f));
					float xSign = rand() % 3 - 1;
					float ySign = rand() % 3 - 1;
					enemy[i].move = D3DXVECTOR3(xSign* randomSpd, ySign * randomSpd, 0.0f);
				}
			}

			// �ړ����� 
			if (enemy[i].tblAdr == moveTbl)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)enemy[i].moveTime;
				float	time = enemy[i].moveTime - index;
				int		size = enemy[i].tblSize;

				float dt = 1.0f / enemy[i].tblAdr[index].frame;	// 1�t���[���Ői�߂鎞��
				enemy[i].moveTime += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					enemy[i].moveTime = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				enemy[i].pos = (1 - time) * enemy[i].tblAdr[index].pos + time * enemy[i].tblAdr[index + 1].pos;

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				enemy[i].rot = (1 - time) * enemy[i].tblAdr[index].rot + time * enemy[i].tblAdr[index + 1].rot;

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				enemy[i].scl = (1 - time) * enemy[i].tblAdr[index].scl + time * enemy[i].tblAdr[index + 1].scl;

			}
			else if (enemy[i].tblAdr == moveTbl2) // ����`�ł̕��
			{

				// �ړ�����
				int		index = (int)enemy[i].moveTime;
				float	time = enemy[i].moveTime - index;
				int		size = enemy[i].tblSize;

				if (index > 0)	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					enemy[i].moveTime = 0.0f;
					index = 0;
				}

				float dt = 1.0f / enemy[i].tblAdr[index].frame;	// 1�t���[���Ői�߂鎞��
				enemy[i].moveTime += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				// ����`�̕��(�x�W�G�Ȑ��H)
				enemy[i].pos = (1 - time) * (1 - time) * (1 - time) * (1 - time) * enemy[i].tblAdr[index].pos
					+ 4 * (1 - time) * (1 - time) * (1 - time) * time * enemy[i].tblAdr[index + 1].pos
					+ 6 * (1 - time) * (1 - time) * time * time * enemy[i].tblAdr[index + 2].pos
					+ 4 * (1 - time) * time * time * time * enemy[i].tblAdr[index + 3].pos
					+ time * time * time * time * enemy[i].tblAdr[index + 4].pos;


			}
			else
			{
				enemy[i].pos += enemy[i].move;
			}

			// �����̍X�V����
			D3DXVECTOR3 diff = enemy[i].pos - pos_old;
			float radian = atan2f(diff.y, diff.x);
			enemy[i].rot.z = radian + D3DX_PI * 0.5f;

			// MAP�O����
			{
				BG *bg = GetBG();

				if ((enemy[i].pos.x < -enemy[i].w) || (enemy[i].pos.x > bg->w + enemy[i].w) ||
					(enemy[i].pos.y < -enemy[i].h) || (enemy[i].pos.y > bg->w + enemy[i].h))
				{
					enemy[i].use = FALSE;
				}
				
			}



			// �v���C���[�Ƃ̓����蔻��
			PLAYER *player = GetPlayer();
			// �V�[���h���������Ă���H
			if (player[0].shieldTime <= 0.0f)
			{// No
				PLAYER *player = GetPlayer();
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(enemy[i].pos, enemy[i].w, enemy[i].h,
							player[j].pos, player[j].w, player[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �h���\��
							SetVibration();
							DestroyEnemy(i);
							player[0].hp -= DAMAGE;

							if (player[0].hp <= 0)
							{
 								player[0].use = FALSE;
								SetFade(FADE_OUT, MODE_RESULT);
							}

						}
					}
				}
			}
		}

#ifdef _DEBUG
		// �f�o�b�O�\��
		if (enemy[i].use == TRUE)
		{
			PrintDebugProc("Enemy No%d  X:%f Y:%f\n", i, enemy[i].pos.x, enemy[i].pos.y);
		}
#endif

	}
	

}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[enemy[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = enemy[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = enemy[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = enemy[i].w;					// �G�l�~�[�̕\����
			float ph = enemy[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				enemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �G�l�~�[�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY *GetEnemy(void)
{

	return &enemy[0];
}

void SetEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false)
		{
			enemy[i].texNo = rand() % TEXTURE_MAX;

			enemy[i].countAnim = 0;
			enemy[i].patternAnim = 0;

			enemy[i].move = D3DXVECTOR3(rand() % 2 * ENEMY_MOVE_SPD, rand() % 2 * ENEMY_MOVE_SPD, 0.0f);

			PLAYER *player = GetPlayer();
			float randomX = (float)((rand() % (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE); // -300�`300
			float randomY = (float)((rand() % (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE); // -300�`300

			enemy[i].pos.x = player[i].pos.x + randomX;
			enemy[i].pos.y = player[i].pos.y + randomY;

			enemy[i].use = true;

			break;
		}
	}
}

void DestroyEnemy(int i)
{
	enemy[i].use = false;

	// �G�t�F�N�g
	SetEffect(EFFECT_EXPLOSION, D3DXVECTOR3(enemy[i].pos.x, enemy[i].pos.y, 0.0f), 100, 100);

	// �X�R�A���Z
	AddScore(ENEMY_DESTORY_SCORE);

	// SE
	PlaySound(SOUND_LABEL_SE_bomb000);
}
