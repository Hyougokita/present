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
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	//InitMeshField(XMFLOAT3(-300.0f, 5.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 10, 10, 13.0f, 13.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �ǂ̏�����
	InitWall();

	// �A�C�e���̏�����
	InitItem();

	// �ǂ̏�����
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//
	//// ��(�����p�̔�����)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

#ifdef DEBUG
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, -25), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), 1, 1, 80.0f, 50.0f);

#endif // DEBUG


	// �؂𐶂₷
	InitTree();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// UI�̏�����
	InitGMUI();

	// �p�[�e�B�N���̏�����
	InitParticle();


	//SetMeshWall(XMFLOAT3(0.0f, 0.0f, -200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), 160.0f, 50.0f);
	//SetMeshWall(XMFLOAT3(160.0f, 0.0f, -200.0f), XMFLOAT3(0.0f, XM_PI / 2, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), 160.0f, 50.0f);

	// �A�C�e���̓����蔻��̏�����
	//SetMeshWall(XMFLOAT3(-160.0f, 0.0f, -200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 160.0f, 50.0f);
	//SetMeshBox(XMFLOAT3(-320.0f, 0.0f, -200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 160.0f, 50.0f,50.0f);

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_sample001);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
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
void UpdateGame(void)
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

	if(g_bPause == false)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �؂̍X�V����
	UpdateTree();

	// �e�̍X�V����
	UpdateBullet();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();

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
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �A�C�e���̕`�揈��
	DrawItem();

	// �e�̕`�揈��
	DrawBullet();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	DrawTree();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();

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


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y +=  GetCamera()->upDown + PLAYER_HEAD_HEIGHT;
								//	���̍���
	


	pos.x -= sinf(GetPlayer()->rot.y + GetCamera()->rot.y) * GetCamera()->len;
	pos.z += cosf(GetPlayer()->rot.y + GetCamera()->rot.y) * GetCamera()->len;
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
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
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();	// �e�̃|�C���^�[��������

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			enemy[i].use = false;
			ReleaseShadow(enemy[i].shadowIdx);


		}
	}


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == false)
			continue;

		// �G�Ɠ������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == false)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].fWidth, enemy[j].size))
			{
				// �����������疢�g�p�ɖ߂�
				bullet[i].use = false;
				ReleaseShadow(bullet[i].shadowIdx);

				// �G�L�����N�^�[�͓|�����
				enemy[j].use = false;
				ReleaseShadow(enemy[j].shadowIdx);


			}
		}

	}

	// �G�l�~�[���S�����S�������ԑJ��
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == false) continue;
		enemy_count++;
	}

	// �G�l�~�[���O�C�H
	if (enemy_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

}

void ChangeViewPort(int num) {
	g_ViewPortType_Game = num;
}


