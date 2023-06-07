//=============================================================================
//

//
//=============================================================================
#include "bullet.h"
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "vibration.h"
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

#define BULLET_SPEED				(3.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/ENEMY/tile_0000.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE ;								// �����Ă��邩�ǂ���
		g_Bullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���S�_����\��
		g_Bullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// rotation
		g_Bullet[i].w   = TEXTURE_WIDTH;						// ��
		g_Bullet[i].h   = TEXTURE_HEIGHT;						// ����
		g_Bullet[i].texNo = 0;									// �e�N�X�`���ԍ�

		g_Bullet[i].countAnim = 0;								// �A�j���J�E���^�[(wait)
		g_Bullet[i].patternAnim = 0;							// �A�j���p�^�[���ԍ�

		g_Bullet[i].move = D3DXVECTOR3(0.0f, -BULLET_SPEED, 0.0f);		// �ړ���

	}

	g_Load = TRUE;												// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		// �g���Ă���e��������������
		if (g_Bullet[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = g_Bullet[i].pos;

			//// �A�j���[�V����
			//g_Bullet[i].countAnim += 1.0f;

			//if (g_Bullet[i].countAnim > ANIM_WAIT)
			//{
			//	g_Bullet[i].countAnim = 0.0f;
			//	// �p�^�[���̐؂�ւ�(1���傫�����Ă����ăp�^�[���̍ő吔�ɂȂ����烊�Z�b�g����A����Ń��[�v�ł���)
			//	g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			//}


			// �ړ� 
			{

				g_Bullet[i].pos += g_Bullet[i].move;


				// MAP�O�`�F�b�N
				BG *bg = GetBG();

				if (g_Bullet[i].pos.x + g_Bullet[i].w / 2 < 0.0f)
				{
					g_Bullet[i].use = FALSE;
				}

				if (g_Bullet[i].pos.x - g_Bullet[i].w / 2> bg->w)
				{
					g_Bullet[i].use = FALSE;
				}

				if (g_Bullet[i].pos.y + g_Bullet[i].h / 2 < 0.0f)
				{
					g_Bullet[i].use = FALSE;
				}

				if (g_Bullet[i].pos.y - g_Bullet[i].h / 2> bg->h)
				{
					g_Bullet[i].use = FALSE;
				}

				// �ړ����I�������v���C���[�Ƃ̓����蔻��
				{
					PLAYER *p = GetPlayer();
					// �V�[���h���������Ă���H
					if (p[0].shieldTime <= 0.0f)
					{// No
						BOOL ans = CollisionBB(p[0].pos, p[0].w, p[0].h, g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							p[0].hp -= DAMAGE;
							SetVibration();
							PlaySound(SOUND_LABEL_SE_hpdrop);

							g_Bullet[i].use = FALSE;

							if (p[0].hp <= 0)
							{
								p[0].use = FALSE;
								SetFade(FADE_OUT, MODE_RESULT);
							}

						}
					}

				}


			}
		}
	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̒e���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[g_Bullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �e�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �e�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �e�̕\����
			float ph = g_Bullet[i].h;		// �e�̕\������


			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W

			float ty = (g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET *GetBullet(void)
{

	return &g_Bullet[0];
}



// �G�̃o���b�g���Z�b�g
void SetBullet(D3DXVECTOR3 pos)
{
	PLAYER *p = GetPlayer();


	for (int i = 0; i < BULLET_MAX; i++)
	{

		if (g_Bullet[i].use == FALSE)
		{
			g_Bullet[i].pos = pos;
			g_Bullet[i].use = TRUE;

			D3DXVECTOR3 vec = p->pos - pos;

			float angle = atan2f(vec.y, vec.x);


			g_Bullet[i].rot.z = angle + D3DX_PI * 0.5f;
			g_Bullet[i].move.x = BULLET_SPEED * cosf(angle);
			g_Bullet[i].move.y = BULLET_SPEED * sinf(angle);
			// �����Ō��ʉ���炷


			break;

		}
	}

}
