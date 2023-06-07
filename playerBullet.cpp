//=============================================================================
//
//
//=============================================================================
#include "playerBullet.h"
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(35)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(35)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define P_BULLET_SPEED				(15.0f) // �e�̕W�����x

#define P_BULLET_OFFSET				(40.0f) // �e�̔��ˎn�_�i�v���C���[����ɂ��Ăǂꂾ������邩�H�j
#define BULLET_SPEED_UP				(20.0f)	// �e�X�s�[�h�A�b�v�A�C�e���擾���̑��x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/PLAYER/player_bullet_001.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static P_BULLET	pBullet[P_BULLET_MAX];	// �v���C���[�o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayerBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			textureName[i],
			NULL,
			NULL,
			&texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;						// 4���_��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < P_BULLET_MAX; i++)
	{
		pBullet[i].use = FALSE ;							// �����Ă��邩�ǂ���
		pBullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �����l
		pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// rotation
		pBullet[i].w   = TEXTURE_WIDTH;						// ��
		pBullet[i].h   = TEXTURE_HEIGHT;					// ����
		pBullet[i].texNo = 0;								// �e�N�X�`���ԍ�

		pBullet[i].upSpeed = BULLET_SPEED_UP;				// �o���b�g�X�s�[�h�A�b�v

		pBullet[i].countAnim = 0;							// �A�j���J�E���^�[(wait)
		pBullet[i].patternAnim = 0;							// �A�j���p�^�[���ԍ�

		pBullet[i].move = D3DXVECTOR3(0.0f, -P_BULLET_SPEED, 0.0f);		// �ړ���

	}

	g_Load = TRUE;											// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayerBullet(void)
{
	if (g_Load == FALSE) return;								//���������Ă��Ȃ����I�����������Ȃ�

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

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayerBullet(void)
{
	for (int i = 0; i < P_BULLET_MAX; i++)
	{
		// �g���Ă���e��������������
		if (pBullet[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = pBullet[i].pos;

			//// �A�j���[�V����
			//pBullet[i].countAnim += 1.0f;

			//if (pBullet[i].countAnim > ANIM_WAIT)
			//{
			//	pBullet[i].countAnim = 0.0f;
			//	// �p�^�[���̐؂�ւ�(1���傫�����Ă����ăp�^�[���̍ő吔�ɂȂ����烊�Z�b�g����A����Ń��[�v�ł���)
			//	pBullet[i].patternAnim = (pBullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			//}


			// �ړ� 
			{

				pBullet[i].pos += pBullet[i].move;


				// MAP�O�`�F�b�N
				BG *bg = GetBG();

				if (pBullet[i].pos.x + pBullet[i].w / 2 < 0.0f)
				{
					pBullet[i].use = FALSE;
				}

				if (pBullet[i].pos.x - pBullet[i].w / 2> bg->w)
				{
					pBullet[i].use = FALSE;
				}

				if (pBullet[i].pos.y + pBullet[i].h / 2 < 0.0f)
				{
					pBullet[i].use = FALSE;
				}

				if (pBullet[i].pos.y - pBullet[i].h / 2> bg->h)
				{
					pBullet[i].use = FALSE;
				}

				// �ړ����I�������v���C���[�Ƃ̓����蔻��
				{
					ENEMY *enemy = GetEnemy();
					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (enemy->use == TRUE)
						{
							BOOL ans = CollisionBB(pBullet[i].pos, pBullet[i].w, pBullet[i].h,
								enemy[j].pos, enemy[i].w, enemy[j].h);

							// �������Ă���H
							if (ans == TRUE)
							{
								DestroyEnemy(j);
								pBullet[i].use = FALSE;
							}
						}
					}
				}
			}

		}

//#ifdef _DEBUG
//		// �f�o�b�O�\��
//		PrintDebugProc("BulletNo%d  X:%f Y:%f\n", i, pBullet[i].pos.x, pBullet[i].pos.y);
//#endif

	}
	
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayerBullet(void)
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
	for (int i = 0; i < P_BULLET_MAX; i++)
	{
		if (pBullet[i].use == TRUE)		// ���̒e���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[pBullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = pBullet[i].pos.x - bg->pos.x;	// �e�̕\���ʒuX
			float py = pBullet[i].pos.y - bg->pos.y;	// �e�̕\���ʒuY
			float pw = pBullet[i].w;		// �e�̕\����
			float ph = pBullet[i].h;		// �e�̕\������


			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (pBullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			
			float ty = (pBullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				pBullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �v���C���[�̒e���Z�b�g
//=============================================================================
void SetPlayerBullet(D3DXVECTOR3 pos, int type)
{

	for (int i = 0; i < P_BULLET_MAX; i++)
	{

		if (pBullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			pBullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			pBullet[i].pos = pos;			// ���W���Z�b�g


			PLAYER *player = GetPlayer();
			// �A�C�e�����������Ă���H
			if (player[0].bulletSpeedUpTime > 0.0f)
			{// Yes
				pBullet[i].upSpeed = P_BULLET_SPEED + BULLET_SPEED_UP;

			}
			else
			{
				pBullet[i].upSpeed = P_BULLET_SPEED;
			}


			switch (type)
			{
			case CHAR_DIR_DOWN:// ������
				pBullet[i].pos += D3DXVECTOR3(0.0f, P_BULLET_OFFSET, 0.0f);		// �O�ʂ���\��
				pBullet[i].move = D3DXVECTOR3(0.0f, pBullet[i].upSpeed, 0.0f);		// �ړ��ʂ�������
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI);
				break;
			case CHAR_DIR_LEFT:// ������
				pBullet[i].pos += D3DXVECTOR3(-P_BULLET_OFFSET, 0.0f, 0.0f);	// �O�ʂ���\��
				pBullet[i].move = D3DXVECTOR3(-pBullet[i].upSpeed, 0.0f, 0.0f);		// �ړ��ʂ�������
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, -D3DX_PI / 2);
				break;
			case CHAR_DIR_RIGHT:// �E����
				pBullet[i].pos += D3DXVECTOR3(P_BULLET_OFFSET, 0.0f, 0.0f);		// �O�ʂ���\��
				pBullet[i].move = D3DXVECTOR3(pBullet[i].upSpeed, 0.0f, 0.0f);		// �ړ��ʂ�������
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI / 2);
				break;
			case CHAR_DIR_UP:// �����
				pBullet[i].pos += D3DXVECTOR3(0.0f, -P_BULLET_OFFSET, 0.0f);	// �O�ʂ���\��
				pBullet[i].move = D3DXVECTOR3(0.0f, -pBullet[i].upSpeed, 0.0f);		// �ړ��ʂ�������
				pBullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				break;
			default:
				break;
			}


			PlaySound(SOUND_LABEL_SE_shot000);
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
P_BULLET *GetPlayerBullet(void)
{

	return &pBullet[0];
}

