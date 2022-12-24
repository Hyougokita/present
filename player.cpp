//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "collision.h"
#include "wall.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define DEBUG

#define	MODEL_PLAYER		"data/MODEL/cone.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_PARTS	"data/MODEL/torus.obj"			// �ǂݍ��ރ��f����

// �v���[���[�̃��f����
#define MODE_BERA_BODY			"data/MODEL/bear_body.obj"
#define MODE_BERA_RIGHT_HAND	"data/MODEL/bear_righthand.obj"
#define MODE_BERA_LEFT_HAND		"data/MODEL/bear_lefthand.obj"
#define MODE_BEAR_HEAD			"data/MODEL/bear_head.obj"
#define MODE_BEAR_RIGHT_LEG		"data/MODEL/bear_rightleg.obj"
#define MODE_BEAR_LEFT_LEG		"data/MODEL/bear_leftleg.obj"

//	����̃��f����
#define MODE_HAND_GUN			"data/MODEL/handgun.obj"

char* playerModelPath[5] =
{ MODE_BERA_RIGHT_HAND,
	MODE_BERA_LEFT_HAND,
	MODE_BEAR_HEAD,
	MODE_BEAR_LEFT_LEG,
	MODE_BEAR_RIGHT_LEG,
};

char* weaponModelPath[1] = {
	MODE_HAND_GUN
};

enum PLAYER_PART_NAME
{
	PLAYER_RIGHT_HAND,
	PLAYER_LEFT_HAND,
	PLAYER_HEAD,
	PLAYER_LEFT_LEG,
	PLAYER_RIGHT_LEG
};


#define MODE_BEAR_RIGHT_LEG2	"data/MODEL/bear_rightleg2.obj"
#define MODE_BEAR_LEFT_LEG2		"data/MODEL/bear_leftleg2.obj"

#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(17.0f)							// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(6)								// �v���C���[�̃p�[�c�̐�
#define PLAYER_WEAPONS_MAX	(1)								// �v���C���[�̃p�[�c�̐�

#define DISTANCE_OF_RAYCAST_PLAYER  (20.0f)

XMFLOAT3 norScl = XMFLOAT3(0.4f, 0.4f, 0.4f);
XMFLOAT3 norScl2 = XMFLOAT3(1.0f, 1.0f, 1.0f);

//�@���߂ă}�E�X���[�h����
bool g_FirstMouseLoad = false;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL CheckTest();
BOOL CheckTestWall();
void SenkeihokanPlayer(PLAYER* player, int type);		//	���`��Ԍv�Z
void DrawPlayerSingle(PLAYER* player);					//	�v���[���[�P�̂̕`��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static PLAYER		g_Weapon[PLAYER_WEAPONS_MAX];	//	�v���[���[�̕���p

static float		roty = 0.0f;

static LIGHT		g_Light;

bool g_isMove = false;


enum PLAYER_HOKAN_TYPE
{
	FULLOUT,
	MOVE_BY_KEYBOARD,
};

#ifdef DEBUG
	BOOL isHitWall = false;
	BOOL bCheckHitWall = false;

	XMFLOAT3 vPosList[8] = {
		XMFLOAT3(-15.0f, 0.0f, 5.0f),
		XMFLOAT3(15.0f, 0.0f, 5.0f),
		XMFLOAT3(-15.0f, 10.0f, 5.0f),
		XMFLOAT3(15.0f, 10.0f, 5.0f),

		XMFLOAT3(-15.0f, 0.0f, -5.0f),
		XMFLOAT3(15.0f, 0.0f, -5.0f),
		XMFLOAT3(-15.0f, 10.0f, -5.0f),
		XMFLOAT3(15.0f, 10.0f, -5.0f),
	};

	static XMFLOAT3 vPos0 = XMFLOAT3(-15.0f, 0.0f, 5.0f);
	static XMFLOAT3 vPos1 = XMFLOAT3(15.0f, 0.0f, 5.0f);
	static XMFLOAT3 vPos2 = XMFLOAT3(-15.0f, 10.0f, 5.0f);
	static XMFLOAT3 vPos3 = XMFLOAT3(15.0f, 10.0f, 5.0f);
	
	static XMFLOAT3 vPos4 = XMFLOAT3(-15.0f, 0.0f, -5.0f);
	static XMFLOAT3 vPos5 = XMFLOAT3(15.0f, 0.0f, -5.0f);
	static XMFLOAT3 vPos6 = XMFLOAT3(-15.0f, 10.0f, -5.0f);
	static XMFLOAT3 vPos7 = XMFLOAT3(15.0f, 10.0f, -5.0f);
	static HITBOX testHitBox;

#endif // DEBUG




	// �v���C���[�̊K�w�A�j���[�V�����f�[�^
	// �v���C���[�̓������E�ɓ������Ă���A�j���f�[�^
	static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_right_leg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_left_leg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f,  -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },
		{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    norScl2, 30 },

	};

	static INTERPOLATION_DATA move_tbl_handgun[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		   norScl2, 30 },

	};



	//�^�C�g���̃A�j���[�V

	static INTERPOLATION_DATA* g_MoveTblAdr[] =
	{
		move_tbl_right,
		move_tbl_left,
		move_tbl_head,
		move_tbl_left_leg,
		move_tbl_right_leg,
		move_tbl_right

	};







//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	WALL* wall = GetWall();
	XMFLOAT3 wallScale = wall[0].scl;
	for (int i = 0; i < 8; i++) {
		
	}

	g_Player.load = TRUE;
	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = XMFLOAT3(-10.0f, PLAYER_OFFSET_Y+50.0f, -200.0f);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.spdValue = VALUE_MOVE;	// �ړ��̊�b�l
	g_Player.font = XMFLOAT3(0.0f,0.0f,0.0f);
	g_Player.use = TRUE;			// true:�����Ă�
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	// �W�����v�p
	g_Player.jumpType = JUMP_NONE;
	g_Player.jumpCnt = 0;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�

	// �L�[�����������̃v���C���[�̌���
	roty = 0.0f;

	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������


	// �K�w�A�j���[�V�����̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		LoadModel(MODE_BERA_BODY, &g_Player.model);
		g_Player.load = true;

		g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
		g_Player.rot = { 0.0f, 0.0f, 0.0f };
		g_Player.scl = XMFLOAT3(0.2f, 0.2f, 0.2f);

		g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

		g_Player.use = true;

		// �����Ńv���C���[�p�̉e���쐬���Ă���
		XMFLOAT3 pos = g_Player.pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ��
		//        ���̃����o�[�ϐ������������e��Index�ԍ�



		// �K�w�A�j���[�V�����p�̏���������
		g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������


		LoadModel(MODE_BERA_RIGHT_HAND, &g_Parts[RIGHT_HAND].model);
		LoadModel(MODE_BERA_LEFT_HAND, &g_Parts[LEFT_HAND].model);
		LoadModel(MODE_BEAR_HEAD, &g_Parts[HEAD].model);
		LoadModel(MODE_BEAR_RIGHT_LEG, &g_Parts[RIGHT_LEG].model);
		LoadModel(MODE_BEAR_LEFT_LEG, &g_Parts[LEFT_LEG].model);
		LoadModel(MODE_HAND_GUN, &g_Parts[5].model);
		// �p�[�c�̏�����
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			g_Parts[i].use = true;
			//LoadModel(MODEL_PLAYER_PARTS, &g_Parts[i].model);

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[i].parent = &g_Player;		// �� �����ɐe�̃A�h���X������
		//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
		//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[i].tblNo = i;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[i].tblMax = 3;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i].load = 1;
		}

		//�@�v���[���[����̏�����
		for (int i = 0; i < PLAYER_WEAPONS_MAX; i++) {
			LoadModel(weaponModelPath[i], &g_Weapon[i].model); // �E�F�|�����f�������[�h����
			g_Weapon[i].parent = &g_Parts[RIGHT_HAND];	// �E�F�|�����E��Ŏ���

			g_Weapon[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Weapon[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Weapon[i].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Weapon[i].use = true;
			g_Weapon[i].load = true;
		}

		//fps�Ȃ̂Ł@�����ז��@false�ɂ���
		g_Parts[PLAYER_HEAD].use = false;
	}



	// �N�H�[�^�j�I���̏�����
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());



	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// �p�[�c�̉������
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// �p�[�c�̉������
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	if (g_FirstMouseLoad == false) {
		//GetCursorPos(&lpPoint);
		//g_MousePos.x = (float)lpPoint.x;
		//g_MousePos.y = (float)lpPoint.y;
		//g_FirstMouseLoad = true;

	}


	CAMERA *cam = GetCamera();
	g_isMove = false;
	g_Player.spd *= 0.9f;

#ifdef DEBUG
	if (CheckTest()) {
		isHitWall = true;
	}
	else {
		isHitWall = false;
	}

	if (CheckTestWall()) {
		bCheckHitWall = true;
	}
	else
	{
		bCheckHitWall = false;
	}
	
#endif // DEBUG



	// �ړ�����
	if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.x -= g_Player.spd;
		roty = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.x += g_Player.spd;
		roty = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.z += g_Player.spd;
		roty = XM_PI;
	}
	if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.z -= g_Player.spd;
		roty = 0.0f;
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}
#endif


	{	// �����������Ƀv���C���[���ړ�������
		// �����������Ƀv���C���[���������Ă��鏊
		
		g_Player.font.y = roty + cam->rot.y;

		g_Player.prePos = g_Player.pos;

		g_Player.pos.x += sinf(g_Player.font.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.font.y) * g_Player.spd;

		if (bCheckHitWall) {
			g_Player.pos = g_Player.prePos;
		}
	}





	// �e���ˏ���
	if (IsMouseLeftTriggered())
	{
		SetBullet(g_Player.pos, g_Player.font);
	}

	// �����̏���
	g_Player.spdValue = VALUE_MOVE;
	if (GetKeyboardPress(DIK_LSHIFT)) {
		g_Player.spdValue = VALUE_MOVE * 2;
	}

	// �W�����v����
	if(GetKeyboardPress(DIK_SPACE)){
		// �W�����v���Ă��Ȃ��ꍇ
		if (g_Player.jumpType == JUMP_NONE) {
			//�@���ʂ̃W�����v
			g_Player.jumpType = JUMP_NORMAL;
		}
	}

	// ���ʂ̃W�����v�̏ꍇ
	if (g_Player.jumpType == JUMP_NORMAL) {
		g_Player.jumpCnt++;
		g_Player.pos.y += (JUMP_POWER - g_Player.jumpCnt * 0.3f);
		//g_Player.pos.y += 20.0f;
		if (g_Player.jumpCnt >= PLAYER_JUMP_COUNT_MAX) {
			g_Player.jumpCnt = 0;
			g_Player.jumpType = JUMP_NONE;
		}
	}

	//�@�d�͂�藎������
	g_Player.pos.y -= G;

	if (g_Player.pos.y <= PLAYER_OFFSET_Y) {
		g_Player.pos.y = PLAYER_OFFSET_Y;
	}

	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 HitPosition;		// ��_
	XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
	bool ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		//g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		//g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		SenkeihokanPlayer(&g_Parts[i], MOVE_BY_KEYBOARD);
	}




	// �|�C���g���C�g�̃e�X�g
	{
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}



	//////////////////////////////////////////////////////////////////////
	// �p������
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;


	g_Player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&g_Player.Quaternion, quat);




#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y);
	PrintDebugProc("isHitWall: %d\n", isHitWall);
	PrintDebugProc("bCheckHitWall: %d\n", bCheckHitWall);
	PrintDebugProc("font.y:%f\n",g_Player.font.y);
	//PrintDebugProc("Mouse Pos: X:%f,Y:%f", g_MousePos.x, g_MousePos.y);
	PrintDebugProc("DisplaySize:%d\n", GetSystemMetrics(SM_CXSCREEN));
	PrintDebugProc("Player Jump Flag:%d\n", g_Player.jumpType);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �N�H�[�^�j�I���𔽉f
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// �����̐ݒ�
	//SetFuchi(1);

	// ���f���`��
	DrawModel(&g_Player.model);



	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		DrawPlayerSingle(&g_Parts[i]);

	}

	//SetFuchi(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

//�@�v���C���[�ƕǂ̓����蔻��
BOOL CheckHitWall() {
	//�@�v���C���[�̈ړ������֎ː������

	return false;
}

BOOL CheckTest() {
	BOOL isHit;
	XMFLOAT3 HitPosition;		// ��_
	XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
	
	XMFLOAT3 startPos = g_Player.pos;	//�@�n�_���v���C���[�̈ʒu�ɂ���
	XMFLOAT3 endPos = g_Player.pos;		
	endPos.z += 10.0f;					//	�e�X�g�Ȃ̂Ł@�I���ʒu���v���C���[�̈ʒu���10.0f���̈ʒu�ɂ���
	
	isHit = RayCast(vPos0, vPos1, vPos2, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}
	isHit = RayCast(vPos1, vPos2, vPos3, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}

	isHit = RayCast(vPos4, vPos5, vPos6, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}
	isHit = RayCast(vPos5, vPos6, vPos7, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}

	return FALSE;
}

BOOL CheckTestWall() {
	MESHWALL* meshwall = GetMeshWall();
	BOOL isHit;
	XMFLOAT3 HitPosition;		// ��_
	XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j

	XMFLOAT3 startPos = g_Player.pos;	//�@�n�_���v���C���[�̈ʒu�ɂ���
	XMFLOAT3 endPos = g_Player.pos;
	//endPos.z -= 30.0f;					//	�e�X�g�Ȃ̂Ł@�I���ʒu���v���C���[�̈ʒu���10.0f���̈ʒu�ɂ���
	
	//float rot = g_Player.rot.y;
	//if (rot == XM_PI) {
	//	endPos.z += DISTANCE_OF_RAYCAST_PLAYER;
	//}
	//else if (rot == 0.0f) {
	//	endPos.z -= DISTANCE_OF_RAYCAST_PLAYER;
	//}
	//else if (rot == XM_PI / 2) {
	//	endPos.x -= DISTANCE_OF_RAYCAST_PLAYER;
	//}
	//else {
	//	endPos.x += DISTANCE_OF_RAYCAST_PLAYER;
	//}

	endPos.x += sinf(g_Player.font.y) * DISTANCE_OF_RAYCAST_PLAYER;
	endPos.z -= cosf(g_Player.font.y) * DISTANCE_OF_RAYCAST_PLAYER;

	for (int i = 0; i < MESHWALL_MAX; i++) {
		if (meshwall[i].use) {
			isHit = RayCast(meshwall[i].vPos[0], meshwall[i].vPos[1], meshwall[i].vPos[2], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return TRUE;
			}
			isHit = RayCast(meshwall[i].vPos[1], meshwall[i].vPos[2], meshwall[i].vPos[3], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return TRUE;
			}
		}
	}


	return FALSE;
}


void SenkeihokanPlayer(PLAYER* player, int type) {
	// �g���Ă���Ȃ珈������
	if ((player->use == true) && (player->tblMax > 0))
	{	// ���`��Ԃ̏���
		int nowNo = (int)player->time;			// �������ł���e�[�u���ԍ������o���Ă���
		int maxNo = player->tblMax;				// �o�^�e�[�u�����𐔂��Ă���
		int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
		INTERPOLATION_DATA* tbl = g_MoveTblAdr[player->tblNo];	// �s���e�[�u���̃A�h���X���擾

		XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
		XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
		XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

		XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
		XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
		XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

		float nowTime = player->time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

		Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
		Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
		Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

		// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
		XMStoreFloat3(&player->pos, nowPos + Pos);

		// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&player->rot, nowRot + Rot);

		// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&player->scl, nowScl + Scl);

		// frame���g�Ď��Ԍo�ߏ���������
		if (type == MOVE_BY_KEYBOARD) {
			if (g_isMove) {
				player->time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)player->time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					player->time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}
			}
			else {
				player->time = 0;
			}
		}
		else if (type == FULLOUT) {
			player->time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)player->time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				player->time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			}
		}
	}
}

void DrawPlayerSingle(PLAYER* player) {
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(player->scl.x, player->scl.y, player->scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(player->rot.x, player->rot.y, player->rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(player->pos.x, player->pos.y, player->pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	if (player->parent != NULL)	// �q����������e�ƌ�������
	{
		mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&player->parent->mtxWorld));
		// ��
		// g_Player.mtxWorld���w���Ă���
	}

	XMStoreFloat4x4(&player->mtxWorld, mtxWorld);

	// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
	if (player->use == true) {
		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);
		// ���f���`��
		DrawModel(&player->model);// ���[���h�}�g���b�N�X�̏�����
	}

}

//�@�^�C�g���p�@�v���C���[�̓����G�l�~�[�Ɍ�����
void TurnHeadTo(PLAYER *head) {

}