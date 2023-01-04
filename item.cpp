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
#define	MODEL_ITEM_AMMO							"data/MODEL/item/ammo.obj"					// �e��{�b�N�X
#define	MODEL_ITEM_HANDGUN						"data/MODEL/item/handgun_table.obj"			// ����̃s�X�g��
#define	MODEL_ITEM_TABLE00						"data/MODEL/item/table00.obj"				// ��
#define	MODEL_ITEM_BOX							"data/MODEL/box.obj"						// ���i�ړ�������@�悹��j
#define MODEL_ITEM_DOOR							"data/MODEL/door.obj"
#define MODEL_ITEM_HOUSE						"data/MODEL/house.obj"
#define MODEL_ITEM_WINDOW						"data/MODEL/wallwindow.obj"
#define MODEL_ITEM_WALLDOORRIGHT				"data/MODEL/walldoorright.obj"
#define MODEL_ITEM_WALLWINDOWDOWN				"data/MODEL/wallwindowdown.obj"
#define MODEL_ITEM_WALLWINDOWLEFT				"data/MODEL/wallwindowleft.obj"
#define MODEL_ITEM_WALLWINDOWRIGHT				"data/MODEL/wallwindowright.obj"
#define MODEL_ITEM_WALLWINDOWUP					"data/MODEL/wallwindowup.obj"
#define MODEL_ITEM_WALLWINDOWUP					"data/MODEL/wallwindowup.obj"
#define MODEL_ITEM_WALLDOORLEFT					"data/MODEL/walldoorleft.obj"
#define MODEL_ITEM_WALLBACK						"data/MODEL/wallback.obj"
#define MODEL_ITEM_WALLLEFT						"data/MODEL/wallleft.obj"
#define MODEL_ITEM_WALLLDOORUP					"data/MODEL/walldoorup.obj"
#define MODEL_ITEM_DOOR							"data/MODEL/door.obj"
#define MODEL_ITEM_DOOR_OPENED					"data/MODEL/dooropened.obj"
#define MODEL_ITEM_ROOF							"data/MODEL/roof.obj"
#define MODEL_ITEM_DOORWAY						"data/MODEL/doorway.obj"

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ITEM_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ITEM_OFFSET_Y		(17.0f)						// �G�l�~�[�̑��������킹��

#define	AMMO_BOX_ADD_NUM	(20)						// �e�{�b�N�X����������e�̐�
#define HOUSE_POS			(XMFLOAT3(-50.0f,0.0f,100.0f))
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
void DrawItemSingle(ITEM* item);
void InitItemWithHitBoxSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, float hitboxW, float hitboxH, float hitboxD, int num, int type);
void InitItemSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use);
void InitItemWithHitBoxFromCsvSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, int dateNum, int num, int type);
void UninitItemSingle(ITEM* item);
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ITEM			g_ItemBullet[ITEM_AMMO_MAX];				// AMMO
static ITEM			g_ItemDecoration[ITEM_DECORATION_TYPE_MAX];	// ����p�A�C�e��
static ITEM			g_ItemHandgun[ITEM_HANDGUN_MAX];			// �s�X�g��
static ITEM			g_ItemBox[ITEM_BOX_MAX];					// �{�b�N�X
static ITEM			g_ItemWindow[ITEM_WINDOW_MAX];				// �Ƃ̃E�B���h
static ITEM			g_ItemHouseWall[ITEM_WALL_MAX];				// �Ƃ̕�
static ITEM			g_ItemDoor[ITEM_HOUSE_DOOR_MAX];			// �Ƃ̃h�A
static ITEM			g_ItemTable[ITEM_TABLE_MAX];				// �A�C�e�����u���Ă����
#ifdef _DEBUG
static ITEM g_ItemTestAmmo[ITEM_TEST_AMMO_MAX];
#endif // _DEBUG


static BOOL			g_Load = FALSE;



// ����p�A�C�e���֘A
// �A�C�e���̃��f���f�[�^
static char* itemDecorationList[ITEM_DECORATION_TYPE_MAX] = {
	MODEL_ITEM_DOOR,
	MODEL_ITEM_HOUSE,
	MODEL_ITEM_ROOF,
	MODEL_ITEM_DOORWAY,
};

static char* itemDoorModelList[ITEM_HOUSE_DOOR_MAX] = {
	MODEL_ITEM_DOOR,
	MODEL_ITEM_DOOR_OPENED,
};

static int doorDataList[ITEM_HOUSE_DOOR_MAX] = {
	DATA_DOOR,
	DATA_DOOR_OPENED
};

//�@�A�C�e���̈ʒu
static XMFLOAT3 itemDecorationPositionList[ITEM_DECORATION_TYPE_MAX] = {
	XMFLOAT3(50.0f,0.0f,0.0f),
	XMFLOAT3(50.0f,0.0f,0.0f),
	HOUSE_POS,
	HOUSE_POS,
};
//	�A�C�e���̃X�J���[
static float itemDecorationScaleList[ITEM_DECORATION_TYPE_MAX] = {
	 3.0f,
	 3.0f,
	 1.0f,
	 1.0f,
};


// hitbox�������� wall
static char* itemHouseWallModelList[ITEM_WALL_MAX] = {
	MODEL_ITEM_WALLDOORRIGHT,
	MODEL_ITEM_WALLWINDOWDOWN,
	MODEL_ITEM_WALLWINDOWLEFT,
	MODEL_ITEM_WALLWINDOWRIGHT,
	MODEL_ITEM_WALLWINDOWUP,
	MODEL_ITEM_WALLDOORLEFT,
	MODEL_ITEM_WALLBACK,
	MODEL_ITEM_WALLLEFT,
	MODEL_ITEM_WALLLDOORUP,
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
#ifdef _DEBUG
	//CSV AMMO TEST
	for (int i = 0; i < ITEM_TEST_AMMO_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 40.0f);
		InitItemWithHitBoxFromCsvSingle(&g_ItemTestAmmo[i], MODEL_ITEM_AMMO, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, 4.0f, true,0,i,ITEM_TYPE_TEST_AMMO);

	}
#endif // _DEBUG
	// �Ƃ̕ǂ̏�����
	for (int i = 0; i < ITEM_WALL_MAX; i++) {
		XMFLOAT3 pos = HOUSE_POS;
		InitItemWithHitBoxFromCsvSingle(&g_ItemHouseWall[i], itemHouseWallModelList[i], true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, 4.0f, true, 2 + i, i, ITEM_TYPE_WALL);
	}

	// ���̏�����
	for (int i = 0; i < ITEM_WINDOW_MAX; i++)
	{
		XMFLOAT3 pos = HOUSE_POS;
		InitItemWithHitBoxFromCsvSingle(&g_ItemWindow[i], MODEL_ITEM_WINDOW, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, ITEM_SIZE, true, 1, i, ITEM_TYPE_WINDOW);
	}

	//�@�h�A�̏�����
	for (int i = 0; i < ITEM_HOUSE_DOOR_MAX; i++) {
		XMFLOAT3 pos = HOUSE_POS;
		pos.x -= 90.0f;
		pos.z -= 8.5f;
		InitItemWithHitBoxFromCsvSingle(&g_ItemDoor[i], itemDoorModelList[i], true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, ITEM_SIZE, true, doorDataList[i], i, ITEM_TYPE_DOOR);
	}
	g_ItemDoor[ITEM_HOUSE_DOOR_OPENED].use = false;

	//	����p�A�C�e���̏�����
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		InitItemSingle(&g_ItemDecoration[i], itemDecorationList[i], true, itemDecorationPositionList[i], XMFLOAT3(0.0f, 0.0f, 0.0f), itemDecorationScaleList[i], 4.0f, true);
	}
	g_ItemDecoration[ITEM_DECORATION_HOUSETEST].use = false;
	g_ItemDecoration[ITEM_DECORATION_DOORTEST].use = false;
	//g_ItemDecoration[ITEM_DECORATION_DOORTEST].pos = HOUSE_POS;



	// ���̏�����
	for (int i = 0; i < ITEM_TABLE_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		InitItemWithHitBoxFromCsvSingle(&g_ItemTable[i], MODEL_ITEM_TABLE00, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.5f, 5.0f, true, DATA_TABLE,i,ITEM_TYPE_TABLE);
	}

	// AMMO�̏�����
	for (int i = 0; i < ITEM_AMMO_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (i == ITEM_AMMO_TABLE) {
			// ����̒e�{�b�N�X
			pos = g_ItemTable[ITEM_TABLE].pos;
			pos.x += 14.0f;
			pos.y += 14.0f;

		}
		InitItemWithHitBoxSingle(&g_ItemBullet[i], MODEL_ITEM_AMMO, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.5f, ITEM_SIZE, true, 28.0f, 8.0f, 18.0f, i, ITEM_AMMO_BOX);

	}

	//�@�s�X�g���̏�����
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// ���̏�s�X�g��
		if (i == ITEM_HANDGUN_TABLE) {
			pos = g_ItemTable[ITEM_TABLE].pos;
			pos.x -= 14.0f;
			pos.y += 14.0f;
		}
		InitItemWithHitBoxSingle(&g_ItemHandgun[i], MODEL_ITEM_HANDGUN, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.2f, ITEM_SIZE, true, 60.0f, 16.0f, 56.0f, i, ITEM_HAND_GUN);
	}

	// �{�b�N�X�̏�����
	for (int i = 0; i < ITEM_BOX_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(-50.0f, 0.0f, 0.0f);
		InitItemWithHitBoxSingle(&g_ItemBox[i], MODEL_ITEM_BOX, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 3.0f, ITEM_SIZE, true, 10.0f, 10.0f, 10.0f, i, ITEM_TYPE_BOX);
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

	// �ǂ̏I��
	for (int i = 0; i < ITEM_WALL_MAX; i++) {
		UninitItemSingle(&g_ItemHouseWall[i]);
	}

	// ���̏I��
	for (int i = 0; i < ITEM_WINDOW_MAX; i++) {
		UninitItemSingle(&g_ItemWindow[i]);
	}

	// �h�A�̏I��
	for (int i = 0; i < ITEM_HOUSE_DOOR_MAX; i++) {
		UninitItemSingle(&g_ItemDoor[i]);
	}

	// ����̏I��
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		UninitItemSingle(&g_ItemDecoration[i]);
	}

	// ���̏I��
	for (int i = 0; i < ITEM_TABLE_MAX; i++) {
		UninitItemSingle(&g_ItemTable[i]);
	}

	// AMMO�̏I��
	for (int i = 0; i < ITEM_AMMO_MAX; i++) {
		UninitItemSingle(&g_ItemBullet[i]);
	}

	// �s�X�g���̏I��
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++) {
		UninitItemSingle(&g_ItemHandgun[i]);
	}

	// �{�b�N�X�̏I��
	for (int i = 0; i < ITEM_BOX_MAX; i++) {
		UninitItemSingle(&g_ItemBox[i]);
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void)
{
#ifdef _DEBUG
	// �h�A����]������
	g_ItemDecoration[ITEM_DECORATION_DOORTEST].rot.y += 0.01f;
#endif // _DEBUG

}



//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
#ifdef _DEBUG
	for (int i = 0; i < ITEM_TEST_AMMO_MAX; i++) {
		if (g_ItemTestAmmo[i].use == false)	continue;
		DrawItemSingle(&g_ItemTestAmmo[i]);
	}
#endif // _DEBUG

	// �Ƃ̕ǂ̕`��
	for (int i = 0; i < ITEM_WALL_MAX; i++) {
		if (g_ItemHouseWall[i].use == false) continue;
		DrawItemSingle(&g_ItemHouseWall[i]);
	}

	//�@�h�A�̕`��
	for (int i = 0; i < ITEM_HOUSE_DOOR_MAX; i++) {
		if (g_ItemDoor[i].use == false) continue;
		DrawItemSingle(&g_ItemDoor[i]);
	}

	// ���̕`��
	for (int i = 0; i < ITEM_TABLE_MAX; i++) {
		if (g_ItemTable[i].use == false) continue;
		DrawItemSingle(&g_ItemTable[i]);
	}

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

	// �{�b�N�X�̕`��
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		if (g_ItemBox[i].use == false) continue;
		DrawItemSingle(&g_ItemBox[i]);
	}

	// ���̕`��
	for (int i = 0; i < ITEM_WINDOW_MAX; i++)
	{
		if (g_ItemWindow[i].use == false) continue;
		DrawItemSingle(&g_ItemWindow[i]);
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

ITEM* GetItemBox() {
	return &g_ItemBox[0];
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
#ifdef _DEBUG
	if (itemType == ITEM_TYPE_TEST_AMMO) {
		g_ItemTestAmmo[num].use = false;
		//GetPlayer()->haveHandgun = true;
		//TurnOnOffUI(UI_HANDGUN, true);
	}
#endif // _DEBUG

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

void InitItemWithHitBoxFromCsvSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, int dateNum, int num, int type) {
	LoadModel(ModelName, &item->model);
	item->load = load;
	item->pos = pos;
	item->rot = rot;
	item->scl = XMFLOAT3(scl, scl, scl);

	item->size = size;	// �����蔻��̑傫��
	item->use = use;
	item->hitBoxIndex = SetMeshBoxFromData(pos, rot, scl, num, type, dateNum);

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

void UninitItemSingle(ITEM* item) {
	if (item->load)
	{
		UnloadModel(&item->model);
		item->load = false;
	}
}