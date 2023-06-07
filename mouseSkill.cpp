//=============================================================================
//
//
//=============================================================================
#include "main.h"
#include "bg.h"
#include "cursor.h"
#include "mouseSkill.h"
#include "sprite.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(MOUSESKILL_DW)		// �}�E�X�X�L���p�̃T�C�Y�i���j
#define TEXTURE_HEIGHT				(MOUSESKILL_DH)		// �}�E�X�X�L���p�̃T�C�Y�i�c�j
#define TEXTURE_MAX					(1)					// �e�N�X�`���̐�
#define WAVESHOT_MOVE_ADDITION		(12.0f)				// �}�E�X�X�L���̉��Z�p�iWAVESHOT�j
#define STUN_SCL_ADDITION			(5.0f)				// �}�E�X�X�L���̉��Z�p�iSTUN�j
#define STUN_SCL_SMOLL				(75.0f)				// �}�E�X�X�L���̉��Z�p�iSTUN�j
#define STUN_SCL_BIG				(300.0f)			// �}�E�X�X�L���̉��Z�p�iSTUN�j
#define HOMING_MOVE_ADDITION		(0.03f)				// �}�E�X�X�L���̉��Z�p�iHOMING�j
#define VARTEX_ROT_MAX				(31.4f)				// �}�E�X�X�L���̉��Z�p�iVOLTEX�j
#define VORTEX_ROT_ADDITION			(0.314f)			// �}�E�X�X�L���̉��Z�p�iVORTEX�j
#define VORTEX_SCL_ADDITION			(2.0f)				// �}�E�X�X�L���̉��Z�p�iVORTEX�j
#define CROSS_SCL_BIG				(300.0f)			// �}�E�X�X�L���̉��Z�p�iCROSS�j
#define CROSS_SCL_ADDITION			(12.0f)				// �}�E�X�X�L���̉��Z�p�iCROSS�j
#define CROSS_INCLINATION			(0.785f)			// �}�E�X�X�L���p�̌X���iCROSS�j
#define CROSS_SWITCH_WIDTH			(0)					// �}�E�X�X�L���̃A�j���[�V�����؂�ւ��p�iCROSS�j
#define CROSS_SWITCH_HEIGHT			(1)					// �}�E�X�X�L���̃A�j���[�V�����؂�ւ��p�iCROSS�j
#define ANIM_WAIT					(120)				// �A�j���[�V�����̐؂�ւ��Wait�l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				vertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView*	texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *texturName[TEXTURE_MAX] = {
	"data/TEXTURE/MOUSE/mouse_Skill.png",

};


static BOOL	isLoad = FALSE;		// ���������s�������̃t���O
static MOUSESKILL	mouseSkill[MOUSESKILL_MAX];	// �}�E�X����p�p�b�h�\����


// ����������
//=============================================================================
HRESULT InitMouseSkill(void)
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

	// �ϐ��̏�����
	for (int i = 0; MOUSESKILL_MAX > i; i++)
	{
		mouseSkill[i].use = FALSE;
		mouseSkill[i].w = TEXTURE_WIDTH;
		mouseSkill[i].h = TEXTURE_HEIGHT;
		mouseSkill[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		mouseSkill[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		mouseSkill[i].texNo = 0;
		mouseSkill[i].countAnim = 0;
		mouseSkill[i].skillFlag = SET_SKILL_MAX;		// ���g�p�̃X�L���t���O���Z�b�g
		mouseSkill[i].skillSwitch = CROSS_SWITCH_WIDTH;	// CROSS�X�L���p�̃t���O���Z�b�g

	}
	isLoad = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMouseSkill(void)
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
void UpdateMouseSkill(void)
{
	// �}�E�X�X�L�������p�֐�
	MouseSkillSelect();			// �� ���̊֐��̒��ɏڍ׋L�q�B�i�X�L���̒ǉ������̊֐����ōs���j
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMouseSkill(void)
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

	for (int i = 0; MOUSESKILL_MAX > i; i++)
	{
		if (mouseSkill[i].use == TRUE && mouseSkill[i].skillFlag != SET_SKILL_MAX)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[mouseSkill[i].texNo]);

			// �}�E�X�X�L���̈ʒu��e�N�X�`���[���W�𔽉f
			float px = mouseSkill[i].pos.x - bg->pos.x;		// �}�E�X����p�p�b�h�̕\���ʒuX
			float py = mouseSkill[i].pos.y - bg->pos.y;		// �}�E�X����p�p�b�h�̕\���ʒuY
			float pw = mouseSkill[i].w;		// �}�E�X����p�p�b�h�̕\����
			float ph = mouseSkill[i].h;		// �}�E�X����p�p�b�h�̕\������

			float tw = 1.0f;			// �e�N�X�`���̕�
			float th = 1.0f;			// �e�N�X�`���̍���
			float tx = 0.0f;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				mouseSkill[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �}�E�X�X�L���\���̂̐擪�A�h���X���擾
//=============================================================================
MOUSESKILL *GetMouseSkill(void)
{
	return &mouseSkill[0];
}

//=============================================================================
// �}�E�X�X�L���̕`��ݒ�
//=============================================================================
void SetMouseSkill(D3DXVECTOR3 pos, int skill)
{
	// ���g�p�̃}�E�X�X�L�������������甭�����Ȃ�
	for (int i = 0; i < MOUSESKILL_MAX; i++)
	{
		if (mouseSkill[i].use == FALSE)		// ���g�p��Ԃ̃}�E�X�X�L����������
		{
			mouseSkill[i].use = TRUE;		// �g�p��Ԃ֕ύX����
			mouseSkill[i].skillFlag = skill;	// �����X�L�����Z�b�g
			mouseSkill[i].pos = pos;		// ���W���Z�b�g
			return;							// �Z�b�g�����̂ŏI������
		}
	}
}

//=============================================================================
// �����X�L���̃Z���N�g
//=============================================================================
void MouseSkillSelect(void)
{
	ENEMY *enemy = GetEnemy();
	int mouseSkillCount = 0;	// ���������}�E�X�X�L���̐�

	for (int i = 0; i < MOUSESKILL_MAX; i++)
	{
		if (mouseSkill[i].use == TRUE)	// �}�E�X�X�L�����g���Ă���H
		{// Yes

			// �A�j���[�V����
			switch (mouseSkill[i].skillFlag)
			{

				// �E�����ɃE�F�[�u�𔭐�����X�L��
			case SET_SKILL_RIGHTSHOT:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.x += WAVESHOT_MOVE_ADDITION;

				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;


				// �������ɃE�F�[�u�𔭐�����X�L��
			case SET_SKILL_LEFTSHOT:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.x -= WAVESHOT_MOVE_ADDITION;

				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;

				// ������ɃE�F�[�u�𔭐�����X�L��
			case SET_SKILL_UPSHOT:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.y -= WAVESHOT_MOVE_ADDITION;

				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;

				// �������ɃE�F�[�u�𔭐�����X�L��
			case SET_SKILL_DOWNSHOT:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;
				mouseSkill[i].pos.y += WAVESHOT_MOVE_ADDITION;

				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_blast);

				break;

				// �N���X���Ċg�傷��X�L��
			case SET_SKILL_CROSS:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;

				mouseSkill[i].rot.z = CROSS_INCLINATION;
				{
					switch (mouseSkill[i].skillSwitch)
					{
					case CROSS_SWITCH_WIDTH:
						// �������Ɋg��
						mouseSkill[i].w += CROSS_SCL_ADDITION;
						if (mouseSkill[i].w > CROSS_SCL_BIG)
						{
							mouseSkill[i].w = TEXTURE_WIDTH;
							mouseSkill[i].skillSwitch = CROSS_SWITCH_HEIGHT;
						}
						break;

					case CROSS_SWITCH_HEIGHT:
						// �c�����Ɋg��
						mouseSkill[i].h += CROSS_SCL_ADDITION;
						if (mouseSkill[i].h > CROSS_SCL_BIG)
						{
							mouseSkill[i].h = TEXTURE_HEIGHT;
							mouseSkill[i].skillSwitch = CROSS_SWITCH_WIDTH;
						}

						// SE
						PlaySound(SOUND_LABEL_SE_shot000);

						break;

					default:
						break;
					}
				}
				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].skillSwitch = CROSS_SWITCH_WIDTH;
					mouseSkill[i].rot.z = 0.0f;
					mouseSkill[i].w = TEXTURE_WIDTH;
					mouseSkill[i].h = TEXTURE_HEIGHT;
					mouseSkill[i].use = FALSE;
				}
				break;

				// ���������G���X�^����Ԃɂ���X�L��
			case SET_SKILL_STUN:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;

				// �������ɏk��
				mouseSkill[i].w -= STUN_SCL_ADDITION;
				if (mouseSkill[i].w < STUN_SCL_SMOLL)
				{
					mouseSkill[i].w = TEXTURE_WIDTH;
				}
				// �c�����Ɋg��
				mouseSkill[i].h += STUN_SCL_ADDITION;
				if (mouseSkill[i].h > STUN_SCL_BIG)
				{
					mouseSkill[i].h = TEXTURE_WIDTH;
				}

				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].w = TEXTURE_WIDTH;
					mouseSkill[i].h = TEXTURE_HEIGHT;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_mouseswip);

				break;

				// �G���z�[�~���O����X�L��
			case SET_SKILL_HOMING:

				// ���Ԃŏ���
				mouseSkill[i].countAnim++;

				//{ // �z�[�~���O���� (���`�⊮�̉e���������A������͕s���Ƃ肠�����擪���z�[�~���O)
				//	for (int k = 0; ENEMY_MAX > k; k++)
				//	{
				//		D3DXVECTOR3 pos = enemy[k].pos - mouseSkill[i].pos;
				//		mouseSkill[i].pos += pos * HOMING_MOVE_ADDITION;
				//	}
				//}

				{ // �z�[�~���O���� (�P��)
					D3DXVECTOR3 pos = enemy->pos - mouseSkill[i].pos;
					mouseSkill[i].pos += pos * HOMING_MOVE_ADDITION;
				}

				// ���̎��Ԃ𒴂����疢�g�p�ɂ���
				if ((mouseSkill[i].countAnim % ANIM_WAIT) == 0)
				{
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_door);

				break;

				// �G���z�����ރX�L��
			case SET_SKILL_VORTEX:

				// �g�債�ĕ\������
				mouseSkill[i].w = TEXTURE_WIDTH * VORTEX_SCL_ADDITION;
				mouseSkill[i].h = TEXTURE_WIDTH * VORTEX_SCL_ADDITION;

				// ��萔�̉�]�ŏ���
				mouseSkill[i].rot.z += VORTEX_ROT_ADDITION;

				// ���i�K�ł͋z�����߂��B�B�B

				// ���̉�]���𒴂����疢�g�p�ɂ���
				if (mouseSkill[i].rot.z > VARTEX_ROT_MAX)
				{
					mouseSkill[i].rot.z = 0.0f;
					mouseSkill[i].skillFlag = SET_SKILL_MAX;
					mouseSkill[i].w = TEXTURE_WIDTH;
					mouseSkill[i].h = TEXTURE_HEIGHT;
					mouseSkill[i].use = FALSE;
				}

				// SE
				PlaySound(SOUND_LABEL_SE_recharge);

				break;


				/* �X�L�����ǉ����ꂽ�炱���ɒǉ����Ă��� */


			default:
				break;
			}


			// ENEMY�Ƃ̓����蔻��
			{
				for (int j = 0; ENEMY_MAX > j; j++)
				{

					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(mouseSkill[i].pos, mouseSkill[i].w, mouseSkill[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);

						switch (ans)
						{
						case TRUE:
						{
							switch (mouseSkill[i].skillFlag)
							{
							case SET_SKILL_RIGHTSHOT:
								// �����������̏���
								DestroyEnemy(j);
								
								break;

							case SET_SKILL_LEFTSHOT:
								// �����������̏���
								DestroyEnemy(j);
								break;

							case SET_SKILL_UPSHOT:
								// �����������̏���
								DestroyEnemy(j);
								break;

							case SET_SKILL_DOWNSHOT:
								// �����������̏���
								DestroyEnemy(j);
								break;

							case SET_SKILL_CROSS:
								// �����������̏���
								DestroyEnemy(j);
								break;

							case SET_SKILL_STUN:
								// �����������̏���
								DestroyEnemy(j);
								break;

							case SET_SKILL_HOMING:
								// �����������̏���
								DestroyEnemy(j);
								break;

							case SET_SKILL_VORTEX:
								// �����������̏���
								DestroyEnemy(j);
								break;

							default:
								break;
							}
						}
						break;

						default:
							break;
						}
					}
				}
			}
		mouseSkillCount++;	// �}�E�X�X�L���̉��Z
		}
	}

}