//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(17.0f)						// �G�l�~�[�̑��������킹��

#define MODE_BERA_BODY			"data/MODEL/bear_body.obj"
#define MODE_BERA_RIGHT_HAND	"data/MODEL/bear_righthand.obj"
#define MODE_BERA_LEFT_HAND		"data/MODEL/bear_lefthand.obj"
#define MODE_BEAR_HEAD			"data/MODEL/bear_head.obj"
#define MODE_BEAR_RIGHT_LEG		"data/MODEL/bear_rightleg.obj"
#define MODE_BEAR_LEFT_LEG		"data/MODEL/bear_leftleg.obj"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void EnemyMoveForward(ENEMY* enemy, XMFLOAT3 vec);
XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY_PARTS		g_EnemyParts[MAX_ENEMY];		// �G�l�~�[�@�p�[�c
static BOOL				g_Load = FALSE;

enum ENEMY_ANIM_TYPE
{
	ENEMY_IDLE,
	ENEMY_MOVE
};

//�@���f���̃t�@�C���p�X
char* modelPath[5] =
{ MODE_BERA_RIGHT_HAND,
	MODE_BERA_LEFT_HAND,
	MODE_BEAR_HEAD,
	MODE_BEAR_LEFT_LEG,
	MODE_BEAR_RIGHT_LEG,
};



// �����̐��`���
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.5f },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },

};

static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_right_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_left_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),				XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),  30},
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};



// IDLE�̐��`���
static INTERPOLATION_DATA idle_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,	ENEMY_OFFSET_Y,  20.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.5f },
	{ XMFLOAT3(0.0f,	ENEMY_OFFSET_Y,  20.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },

};

static INTERPOLATION_DATA idle_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -XM_PI / 8), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_right_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI / 8),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_left_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),					XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI / 16, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 16, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

// �����̃A�j���[�V
static INTERPOLATION_DATA* move_tblAdr[] = {
	move_tbl_right,
	move_tbl_left,
	move_tbl_head,
	move_tbl_left_leg,
	move_tbl_right_leg,
};


// IDLE�̃A�j���[�V
static INTERPOLATION_DATA* idle_tblAdr[] = {
	idle_tbl_right,
	idle_tbl_left,
	idle_tbl_head,
	idle_tbl_left_leg,
	idle_tbl_right_leg,
};

//�@move�̃A�j���[�V�̃T�C�Y
static int move_tblSize[MAX_ENMEY_PARTS] = {
	sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_left_leg) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_right_leg) / sizeof(INTERPOLATION_DATA),
};

//�@idle�̃A�j���[�V�̃T�C�Y
static int idle_tblSize[MAX_ENMEY_PARTS] = {
	sizeof(idle_tbl_right) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_left) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_head) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_left_leg) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_right_leg) / sizeof(INTERPOLATION_DATA),
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	// �{��
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODE_BERA_BODY, &g_Enemy[i].model);
		g_Enemy[i].load = true;
		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.2f, 0.2f, 0.2f);
		g_Enemy[i].spd = 1.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].Parent = NULL;

		g_Enemy[i].actType = ENEMY_IDLE;

		g_Enemy[i].use = true;			// true:�����Ă�

		// ���ꂩ��@�p�[�c�̏���
		for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
			// ���f���̓ǂݍ���
			LoadModel(modelPath[j], &g_EnemyParts[i].Parts[j].model);
			g_EnemyParts[i].Parts[j].load = true;

			//�@�ʒu�@��]�@�g��k��
			g_EnemyParts[i].Parts[j].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
			g_EnemyParts[i].Parts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyParts[i].Parts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			//�@���`���
			g_EnemyParts[i].Parts[j].move_time = 0.0f;
			g_EnemyParts[i].Parts[j].tbl_adr = idle_tblAdr[j];
			g_EnemyParts[i].Parts[j].tbl_size = idle_tblSize[j];
			//�@�����Ă��邩�ǂ���
			g_EnemyParts[i].Parts[j].use = true;

			//	�e�q�֌W
			g_EnemyParts[i].Parts[j].Parent = &g_Enemy[i];
		}
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER *player = GetPlayer();
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_Enemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Enemy[i].move_time;
				float	time = g_Enemy[i].move_time - index;
				int		size = g_Enemy[i].tbl_size;

				float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Enemy[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Enemy[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			}
			else {
				XMFLOAT3 vec = MakeUnitVector(g_Enemy[i].pos, player->pos);
				EnemyMoveForward(&g_Enemy[i], vec);
			}

			
			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Enemy[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Enemy[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}


			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}

	/*�p�[�c*/

	//�@�{�̂̐�
	for (int i = 0; i < MAX_ENEMY; i++) {
		//�@�p�[�c�̐�
		for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
			// �p�[�c�������Ă���ꍇ
			if (g_EnemyParts[i].Parts[j].use) {
				/*�s����Ԃ̔��f*/

				// IDLE�̂Ƃ�
				if (g_Enemy[i].actType == ENEMY_IDLE) {
					for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
						g_EnemyParts[i].Parts[j].tbl_adr = idle_tblAdr[j];
						g_EnemyParts[i].Parts[j].tbl_size = idle_tblSize[j];
					}
				}
				else if (g_Enemy[i].actType == ENEMY_MOVE) {
					for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
						g_EnemyParts[i].Parts[j].tbl_adr = move_tblAdr[j];
						g_EnemyParts[i].Parts[j].tbl_size = move_tblSize[j];
					}
				}

				{	// ���`��Ԃ̏���
					int nowNo = (int)g_EnemyParts[i].Parts[j].move_time;		// �������ł���e�[�u���ԍ������o���Ă���
					int maxNo = g_EnemyParts[i].Parts[j].tbl_size;				// �o�^�e�[�u�����𐔂��Ă���
					int nextNo = (nowNo + 1) % maxNo;							// �ړ���e�[�u���̔ԍ������߂Ă���
					INTERPOLATION_DATA* tbl = g_EnemyParts[i].Parts[j].tbl_adr;	// �s���e�[�u���̃A�h���X���擾

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

					float nowTime = g_EnemyParts[i].Parts[j].move_time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

					Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
					Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
					Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

					// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
					XMStoreFloat3(&g_EnemyParts[i].Parts[j].pos, nowPos + Pos);

					// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_EnemyParts[i].Parts[j].rot, nowRot + Rot);

					// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_EnemyParts[i].Parts[j].scl, nowScl + Scl);

					// frame���g�Ď��Ԍo�ߏ���������

					g_EnemyParts[i].Parts[j].move_time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
					if ((int)g_EnemyParts[i].Parts[j].move_time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
					{
						g_EnemyParts[i].Parts[j].move_time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
					}
				}
			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������

	// �L�[K�������ƈړ���ԂɕύX����
	if (GetKeyboardTrigger(DIK_K)) {
		for (int i = 0; i < MAX_ENEMY; i++) {
			g_Enemy[i].actType = ENEMY_MOVE;
		}
	}

	// �L�[J��������IDLE��ԂɕύX����
	if (GetKeyboardTrigger(DIK_J)) {
		for (int i = 0; i < MAX_ENEMY; i++) {
			g_Enemy[i].actType = ENEMY_IDLE;
		}
	}

#endif
}



//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Enemy[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);



		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Enemy[i].model);
		for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_EnemyParts[i].Parts[j].scl.x, g_EnemyParts[i].Parts[j].scl.y, g_EnemyParts[i].Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyParts[i].Parts[j].rot.x, g_EnemyParts[i].Parts[j].rot.y, g_EnemyParts[i].Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_EnemyParts[i].Parts[j].pos.x, g_EnemyParts[i].Parts[j].pos.y, g_EnemyParts[i].Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (&g_EnemyParts[i].Parts[j].Parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_EnemyParts[i].Parts[j].Parent->mtxWorld));
				// ��
				// g_Player.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_EnemyParts[i].Parts[j].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_EnemyParts[i].Parts[j].use == false) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_EnemyParts[i].Parts[j].model);
		}
	}



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//�@�G�l�~�[����������Ɉړ�������
void EnemyMoveForward(ENEMY *enemy, XMFLOAT3 vec) {
	enemy->pos.x += vec.x * enemy->spd;
	enemy->pos.z += vec.z * enemy->spd;

	//�p������
	XMFLOAT3 Normal;
	Normal = { -vec.x,0.0f,vec.z };

	XMVECTOR vx, nvx, front;
	XMVECTOR quat;
	float len, angle;


	enemy->XVector = Normal;

	//front = { 0.0f, 0.0f, 1.0f, 0.0f };
	if (vec.z > 0) {
		enemy->rot.y = XMConvertToRadians(180.0f);
		front = { 0.0f, 0.0f, -1.0f, 0.0f };
	}
	else {
		enemy->rot.y = XMConvertToRadians(0.0f);
		front = { 0.0f, 0.0f, 1.0f, 0.0f };
	}


	vx = XMVector3Cross(front, XMLoadFloat3(&enemy->XVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);

	angle = asinf(len);

	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&enemy->Quaternion), quat, 0.05f);
	XMStoreFloat4(&enemy->Quaternion, quat);
}


// ���_�̒P�ʃx�N�g�������߂�
XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end) {
	XMFLOAT3 vec = XMFLOAT3(0.0f,0.0f,0.0f);
	float disX = end.x - start.x;
	float disZ = end.z - start.z;

	float distance = pow((pow(disX, 2) + pow(disZ, 2)), 0.5);
	vec.x = disX / distance;
	vec.z = disZ / distance;


	return vec;
}
