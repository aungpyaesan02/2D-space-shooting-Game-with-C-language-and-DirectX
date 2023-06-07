//=============================================================================
//

//
//=============================================================================

#include "file.h"


/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
SAVEDATA	save;		// �Z�[�u�f�[�^�쐬�ꏊ



//=============================================================================
// �Z�[�u�f�[�^�ǂݍ���
//=============================================================================

void LoadHighscore(void)
{
	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE* fp;

	printf("\n���[�h�J�n�E�E�E");
	fp = fopen("savedata.bin", "rb");	// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����

	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
	{
		fread(&save, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X�֎w�肵���o�C�g�����t�@�C������ǂݍ���
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}


	int *highScore = GetHighScore();
	for (int i = 0; i < HIGHSCORE_MAX; i++)
	{

		highScore[i] = save.highScore[i];
	}

}

//=============================================================================
// �Z�[�u�f�[�^�쐬���t�@�C���o��
//=============================================================================

void SaveHighscore(void)
{

	
	int *highScore = GetHighScore();

	for (int i = 0; i < HIGHSCORE_MAX; i++)
	{

		save.highScore[i] = highScore[i];
	}



	// SAVEDATA�\���̂��ƑS�����t�@�C���ɏo�͂���
	FILE *fp;
	
	
	printf("\n�Z�[�u�J�n�E�E�E");
	fp = fopen("savedata.bin", "wb");			// �t�@�C�����o�C�i���������݃��[�h��Open����

	if (fp != NULL)								// �t�@�C��������Ώ������݁A������Ζ���
	{
		fwrite(&save, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else

	{
		printf("�t�@�C���G���[�I\n");
	}

}