//=============================================================================
//
// �G�l�~�[���f������ [item.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "item.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "meshwall.h"
#include "wall.h"
#include "bullet.h"
#include "gamemodeUI.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ITEM_AMMO							"data/MODEL/item/ammo.obj"					// �ǂݍ��ރ��f����
#define	MODEL_ITEM_HANDGUN						"data/MODEL/item/handgun_table.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ITEM_TABLE00						"data/MODEL/item/table00.obj"				// �ǂݍ��ރ��f����
#define	MODEL_ITEM_TABLE_AMMO					"data/MODEL/item/table01.obj"				// �ǂݍ��ރ��f����
#define	MODEL_ITEM_TABLE_HANDGUN				"data/MODEL/item/table02.obj"				// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ITEM_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ITEM_OFFSET_Y		(17.0f)						// �G�l�~�[�̑��������킹��

#define	AMMO_BOX_ADD_NUM	(20)						// �e�{�b�N�X����������e�̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
void DrawItemSingle(ITEM* item);
void InitItemWithHitBoxSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, float hitboxW, float hitboxH, float hitboxD, int num, int type);
void InitItemSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use);
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ITEM			g_ItemBullet[ITEM_AMMO_MAX];				// AMMO
static ITEM			g_ItemDecoration[ITEM_DECORATION_TYPE_MAX];	// ����p�A�C�e��
static ITEM			g_ItemHandgun[ITEM_HANDGUN_MAX];			// �s�X�g��

static BOOL			g_Load = FALSE;


static char* itemDecorationList[ITEM_DECORATION_TYPE_MAX] = {
	MODEL_ITEM_TABLE00,
};


static XMFLOAT3 itemDecorationPositionList[ITEM_DECORATION_TYPE_MAX] = {
	XMFLOAT3(0.0f,0.0f,0.0f),
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{


	//	����p�A�C�e���̏�����
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		InitItemSingle(&g_ItemDecoration[i], itemDecorationList[i], true, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 0.5f, 4.0f, true);
	}



	// AMMO�̏�����
	for (int i = 0; i < ITEM_AMMO_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (i == ITEM_AMMO_TABLE) {
			pos = itemDecorationPositionList[ITEM_DECORATION_TABLE00];
			pos.x += 14.0f;
			pos.y += 14.0f;

		}
		InitItemWithHitBoxSingle(&g_ItemBullet[i], MODEL_ITEM_AMMO, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.5f, ITEM_SIZE, true, 28.0f, 8.0f, 18.0f, i, ITEM_AMMO_BOX);

	}

	//�@�s�X�g���̏�����
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (i == ITEM_HANDGUN_TABLE) {
			pos = itemDecorationPositionList[ITEM_DECORATION_TABLE00];
			pos.x -= 14.0f;
			pos.y += 14.0f;
		}
		InitItemWithHitBoxSingle(&g_ItemHandgun[i], MODEL_ITEM_HANDGUN, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.2f, ITEM_SIZE, true, 60.0f, 16.0f, 56.0f, i, ITEM_HAND_GUN);
	}

	//g_ItemBullet[ITEM_AMMO_TABLE].pos = g_ItemDecoration[ITEM_DECORATION_TABLE00].pos;
	//g_ItemHandgun[ITEM_HANDGUN_TABLE].pos = g_ItemDecoration[ITEM_DECORATION_TABLE00].pos;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ITEMBULLET; i++)
	{
		if (g_ItemBullet[i].load)
		{
			UnloadModel(&g_ItemBullet[i].model);
			g_ItemBullet[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void)
{
	
}



//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
{


	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �e�{�b�N�X�̕`��
	for (int i = 0; i < ITEM_AMMO_MAX; i++)
	{
		if (g_ItemBullet[i].use == false) continue;
		DrawItemSingle(&g_ItemBullet[i]);
	}

	// �s�X�g���̕`��
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		if (g_ItemHandgun[i].use == false) continue;
		DrawItemSingle(&g_ItemHandgun[i]);
	}

	//�@����p�A�C�e���̕`��
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		if (g_ItemDecoration[i].use == false) continue;
		DrawItemSingle(&g_ItemDecoration[i]);
	}



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ITEM *GetItemBullet()
{
	return &g_ItemBullet[0];
}

//�@�w�肳�ꂽ�A�C�e���𖳌�������
void DestoryItem(int num, char itemType) {
	if (itemType == ITEM_AMMO_BOX) {
		g_ItemBullet[num].use = false;
		AddSubMagazine(AMMO_BOX_ADD_NUM, WEAPON_HANDGUN);
	}
	if (itemType == ITEM_HAND_GUN) {
		g_ItemHandgun[num].use = false;
		GetPlayer()->haveHandgun = true;
		TurnOnOffUI(UI_HANDGUN, true);

	}
}

void DrawItemSingle(ITEM* item) {
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(item->scl.x, item->scl.y, item->scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(item->rot.x, item->rot.y + XM_PI, item->rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(item->pos.x, item->pos.y, item->pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&item->mtxWorld, mtxWorld);

	DrawModel(&item->model);
}

// �q�b�g�{�b�N�X�t���̃A�C�e���̏�����
void InitItemWithHitBoxSingle(ITEM* item, char* ModelName,bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use,float hitboxW,float hitboxH,float hitboxD, int num, int type) {
	LoadModel(ModelName, &item->model);
	item->load = load;
	item->pos = pos;
	item->rot = rot;
	item->scl = XMFLOAT3(scl, scl, scl);

	item->size = size;	// �����蔻��̑傫��
	item->use = use;
	item->hitBoxIndex = SetMeshBox(pos, rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), hitboxW * scl, hitboxH * scl, hitboxD * scl, num, type);

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&item->model, &item->diffuse[0]);
}

//�@�q�b�g�{�b�N�X�Ȃ��̃A�C�e���̏�����
void InitItemSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use) {
	LoadModel(ModelName, &item->model);
	item->load = load;
	item->pos = pos;
	item->rot = rot;
	item->scl = XMFLOAT3(scl, scl, scl);

	item->size = size;	// �����蔻��̑傫��
	item->use = use;
	item->hitBoxIndex = -99;

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&item->model, &item->diffuse[0]);
}