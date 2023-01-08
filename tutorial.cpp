//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "wall.h"
#include "gamemodeUI.h"
#include "item.h"
#include "tutorial.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define DEBUG

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static bool	g_bPause = true;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �ǂ̏�����
	InitWall();

	// �A�C�e���̏�����
	InitItem();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// UI�̏�����
	InitGMUI();

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_sample001);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial (void)
{
	// �p�[�e�B�N���̏I������
	UninitParticle();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	UninitBullet();

	// �؂̏I������
	UninitTree();

	// UI�̏I������
	UninitGMUI();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

	// �ǂ̏I��
	UninitWall();

	// �A�C�e���̏I������
	UninitItem();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTutorial(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? false : true;
	}


#endif

	if (g_bPause == false)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �v���C���[�̍X�V����
	UpdatePlayer();



	// �Ǐ����̍X�V
	UpdateMeshWall();



	// �e�̍X�V����
	UpdateBullet();



	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateScore();

	// �ǂ̍X�V
	UpdateWall();

	// �A�C�e���̍X�V����
	UpdateItem();

	// UI�̍X�V����
	UpdateGMUI();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();



	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �A�C�e���̕`�揈��
	DrawItem();

	// �e�̕`�揈��
	DrawBullet();

	// �ǂ̕`�揈��
	DrawMeshWall();



	// �ǂ̕`��
	DrawWall();


	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(false);

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	// �X�R�A�̕`�揈��
	DrawScore();

	// UI�̕`�揈��
	DrawGMUI();


	// ���C�e�B���O��L����
	SetLightEnable(true);

	// Z��r����
	SetDepthEnable(true);
}


void DrawTutorial(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y += GetCamera()->upDown + PLAYER_HEAD_HEIGHT;
	//	���̍���



	pos.x -= sinf(GetPlayer()->rot.y + GetCamera()->rot.y) * GetCamera()->len;
	pos.z += cosf(GetPlayer()->rot.y + GetCamera()->rot.y) * GetCamera()->len;
	SetCameraAT(pos);
	SetCamera();

	switch (g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawTutorial0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		//SetViewPort(TYPE_LEFT_HALF_SCREEN);
		//DrawGame0();

		// �G�l�~�[���_
		//pos = GetPlayer()->pos;
		pos = XMFLOAT3(171.0f, 27.0f, -356.0f);
		//pos.y = 0.0f;
		SetCameraAT2(pos);
		SetCamera();
		SetViewPort(TYPE_FULL_SCREEN);
		DrawTutorial0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawTutorial0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawTutorial0();
		break;

	}

}






