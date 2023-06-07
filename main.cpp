//=============================================================================
//

//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "title.h"
#include "button.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "item.h"
#include "result.h"
#include "sound.h"
#include "fade.h"
#include "file.h"
#include "pause.h"
#include "effect.h"
#include "bullet.h"
#include "stageSelect.h"

#include "cursor.h"
#include "mousePad.h"
#include "mouseDraw.h"
#include "mouseSkill.h"
#include "vibration.h"
#include "tutorial.h"
#include "ui.h"
#include "playerBullet.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME			"AppClass"					// �E�C���h�E�̃N���X��
#define WINDOW_NAME			"Stellar Draw Shooter"	// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
void CheckGameClear(void);

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long mousePosX = 0;
long mousePosY = 0;


#ifdef _DEBUG
int		cntFPS;							// FPS�J�E���^
char	debugStr[2048] = WINDOW_NAME;	// �f�o�b�O�����\���p

#endif

int gameMode = MODE_TITLE;				// �N�����̉�ʂ�ݒ�


BOOL isPause = FALSE;					// �|�[�Y���Ȃ�true

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	//UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// �E�B���h�E�̍����W
						CW_USEDEFAULT,																		// �E�B���h�E�̏���W
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// �E�B���h�E����
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
						NULL,
						NULL,
						hInstance,
						NULL);

	// �E�B���h�E���[�h���t���X�N���[�����[�h���̏���
	bool mode = true;

	int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// Yes�Ȃ�Window���[�h�ŋN��
		mode = true;
		break;
	case IDNO:		// No�Ȃ�t���X�N���[�����[�h�ŋN��
		mode = false;
		break;
	case IDCANCEL:	// CANCEL�Ȃ�I��
	default:
		return -1;
		break;
	}

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;
	
	// �E�C���h�E�̕\��(Init()�̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// �V�X�e���������擾

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				cntFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(debugStr, WINDOW_NAME);
				wsprintf(&debugStr[strlen(debugStr)], " FPS:%d", cntFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&debugStr[strlen(debugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, debugStr);
#endif

				dwFrameCount++;		// �����񐔂̃J�E���g�����Z
			}
		}
	}
	
	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
			break;
		}
		break;

	case WM_MOUSEMOVE:
		mousePosX = LOWORD(lParam);
		mousePosY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_���̏�����
	srand((unsigned int)time(NULL));

	// �����_�����O�����̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	// �J���������̏�����
	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�����̏�����
	InitSound(hWnd);

	// �t�F�[�h�����̏�����
	InitFade();

	// �ŏ��̃��[�h���Z�b�g���āA���̒��ŏ���������
	SetMode(gameMode);

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_END);

	// �t�F�[�h�̏I������
	UninitFade();

	// �T�E���h�̏I������
	UninitSound();

	// ���͂̏I������
	UninitInput();

	// �J�����̏I������
	UninitCamera();

	// �����_�����O�̏I������
	UninitRenderer();

}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����̍X�V����
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (gameMode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		UpdateButton();
		break;

	case MODE_TUTORIAL:
		UpdateTitle();
		UpdateTutorial();
		break;

	case MODE_STAGE_SELECT: // �X�e�[�W�Z���N�g��ʂ̍X�V
		UpdateStageSelect();
		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V
		UpdatePause();

		if (GetPause() == FALSE)	// �|�[�Y��Ԃ���Ȃ��ꍇ�̂ݍX�V
		{
			UpdateBG();
			UpdatePlayer();
			UpdateEnemy();
			UpdateBullet();
			UpdateScore();
			UpdateEffect();
			UpdateItem();

			UpdateMousePad();
			UpdateCursor();
			UpdateMouseDraw();
			UpdateMouseSkill();
			UpdateVibration();
			UpdateUI();
			UpdatePlayerBullet();

			CheckGameClear();	// �Q�[���N���A���`�F�b�N����
		}

		break;

	case MODE_RESULT:		// ���U���g��ʂ̍X�V
		UpdateResult();
		break;

	}

	UpdateFade();			// �t�F�[�h�̍X�V����
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// �J�������Z�b�g
	SetCamera();

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);
	
	// ���[�h�ɂ���ď����𕪂���
	switch (gameMode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		DrawTitle();
		DrawButton();
		break;

	case MODE_TUTORIAL:
		DrawTitle();
		DrawTutorial();
		break;

	case MODE_STAGE_SELECT: // �X�e�[�W�Z���N�g��ʂ̕`��
		DrawStageSelect();
		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��
		DrawBG();
		DrawItem();
		DrawEnemy();
		DrawPlayerBullet();
		DrawPlayer();
		DrawPause();
		DrawBullet();
		DrawScore();
		DrawEffect();

		DrawMousePad();
		DrawMouseDraw();
		DrawMouseSkill();
		DrawCursor();
		DrawVibration();
		DrawUI();
		
		break;

	case MODE_RESULT:		// ���U���g��ʂ̕`��
		DrawResult();
		break;
	}

	DrawFade();				// �t�F�[�h��ʂ̕`��


#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// �Ȃ��~�߂�
	StopSound();

	// ���[�h��ς���O�Ƀ����������ׂĊJ��
	UninitTitle();
	UninitTutorial();
	UninitItem();
	UninitStageSelect();
	UninitButton();
	UninitBG();
	UninitPlayer();
	UninitEnemy();
	UninitBullet();
	UninitPause();
	UninitEffect();
	UninitResult();
	UninitScore();
	UninitEffect();
	UninitMousePad();
	UninitCursor();
	UninitMouseDraw();
	UninitMouseSkill();
	UninitVibration();
	UninitUI();
	UninitPlayerBullet();

	gameMode = mode;	// ���̃��[�h���Z�b�g

	switch (gameMode)
	{
	case MODE_TITLE:		// �^�C�g�����
		InitTitle();
		InitButton();
		
		// BGM
		PlaySound(SOUND_LABEL_BGM_sample003);
		break;

	case MODE_TUTORIAL:
		InitTitle();
		InitTutorial();
		// BGM
		PlaySound(SOUND_LABEL_BGM_sample000); // �`���[�g���A���p��BGM
		break;

	case MODE_STAGE_SELECT: // �X�e�[�W�Z���N�g���
		InitStageSelect();
		
		// BGM
		PlaySound(SOUND_LABEL_BGM_selectscreen);		
		break;
	case MODE_GAME:			// �Q�[�����
		InitBG();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitPause();
		InitScore();
		InitEffect();
		InitItem();
		InitMousePad();
		InitCursor();
		InitMouseDraw();
		InitMouseSkill();
		InitVibration();
		InitUI();
		InitPlayerBullet();

		//// BGM
		PlaySound(SOUND_LABEL_BGM_sample005);
		break;

	case MODE_RESULT:		// ���U���g���
		InitResult();
		// BGM
		PlaySound(SOUND_LABEL_BGM_sample004);
		break;

	case MODE_END:			// �Q�[���I��
		StopSound();
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return gameMode;
}


//=============================================================================
// �}�E�X���W�̎擾
//=============================================================================
long GetMousePosX(void)
{
	return mousePosX;
}

long GetMousePosY(void)
{
	return mousePosY;
}


#ifdef _DEBUG
char *GetDebugStr(void)
{
	return debugStr;
}
#endif


//=============================================================================
// �Q�[���N���A���`�F�b�N����
//=============================================================================
void CheckGameClear(void)
{
	if (GetKeyboardTrigger(DIK_F3))
	{
		// ���U���g�ɃX�R�A���Z�b�g����
		SetResult(GetScore());
		SaveHighscore();
		SetFade(FADE_OUT, MODE_RESULT);	// ���U���g��ʂ�
	}
}


//=============================================================================
// �Q�[���̃|�[�Y��Ԃ��擾�E�ݒ肷��
//=============================================================================
BOOL GetPause(void)
{
	return isPause;
}

void SetPause(BOOL flg)
{
	isPause = flg;
}

