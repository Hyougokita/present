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
#include "collision.h"
#include "debugproc.h"
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
#define MODEL_ITEM_CASTLEWALL					"data/MODEL/castlewall.obj"
#define MODEL_ITEM_GATE							"data/MODEL/gate.obj"
#define MODEL_ITEM_CONTROLLER_OFF				"data/MODEL/controlleroff.obj"
#define MODEL_ITEM_CONTROLLER_ON				"data/MODEL/controlleron.obj"

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ITEM_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ITEM_OFFSET_Y		(17.0f)						// �G�l�~�[�̑��������킹��

#define	AMMO_BOX_ADD_NUM	(20)						// �e�{�b�N�X����������e�̐�
#define HOUSE_POS			(XMFLOAT3(350.0f,0.0f,-400.0f))

#define CASTLE_WALL_NUMBER		(13)					//�@�ǂ̕Еӂ̐�
#define GATE_WALL_NUMER			(5)						//	�Q�[�g���ӂ̕Еӂ̕ǂ̐�
#define GATE_WALL_NUMER2		(2)
#define GATE_WALL_POS			(XMFLOAT3(MAP_TOP- 50.0f, 0.0f, -100.0f))

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
static ITEM			g_CastleWall[CASTLE_WALL_NUMBER * 4];		// �}�b�v���ӂ̕�
static ITEM			g_GateWall[GATE_WALL_NUMER * 2];			// �Q�[�g���ӂ̕�
static ITEM			g_ItemController[ITEM_CONTROLLER_MAX];		// �Q�[�g���R���g���[������R���g���[���[
#ifdef _DEBUG
static ITEM g_ItemTestAmmo[ITEM_TEST_AMMO_MAX];
#endif // _DEBUG

//  �Ƃ̈ʒu
XMFLOAT3 g_HousePos = HOUSE_POS;

//  �{�b�N�X�̈ʒu
XMFLOAT3 g_BoxPos = XMFLOAT3(g_HousePos.x + 90.0f, 0.0f, g_HousePos.z - 20.0f);

static BOOL			g_Load = FALSE;

//	�h�A�J�J��Ԃ��̖h�~
bool g_CanOpenDoor = true;
int  g_OpenDoorCount = 0;
#define OPENDOOR_CD	(20)

// �R���g���[���[
bool g_CanOpenController = true;
int  g_OpenControllerCount = 0;
#define OPENCONTROLLER_CD	(15)

// ����p�A�C�e���֘A
// �A�C�e���̃��f���f�[�^
static char* itemDecorationList[ITEM_DECORATION_TYPE_MAX] = {
	MODEL_ITEM_DOOR,
	MODEL_ITEM_HOUSE,
	MODEL_ITEM_ROOF,
	MODEL_ITEM_DOORWAY,
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

//	�Ƃ̃h�A�֘A
static char* itemDoorModelList[ITEM_HOUSE_DOOR_MAX] = {
	MODEL_ITEM_DOOR,
	MODEL_ITEM_DOOR_OPENED,
};

static int doorDataList[ITEM_HOUSE_DOOR_MAX] = {
	DATA_DOOR,
	DATA_DOOR_OPENED
};

static char* itemControllerModelList[ITEM_CONTROLLER_MAX] = {
	MODEL_ITEM_CONTROLLER_OFF,
	MODEL_ITEM_CONTROLLER_ON,
};

static int controllerDataList[ITEM_CONTROLLER_MAX] = {
	DATA_CONTROLLER_OFF,
	DATA_CONTROLLER_ON,
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

bool g_boxHit = false;


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

	// �R���g���[���[�̏�����
	for (int i = 0; i < ITEM_CONTROLLER_MAX; i++) {
		XMFLOAT3 pos = HOUSE_POS;
		InitItemWithHitBoxFromCsvSingle(&g_ItemController[i], itemControllerModelList[i], true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, ITEM_SIZE, true, controllerDataList[i], i, ITEM_TYPE_CONTROLLER);
	}


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
		XMFLOAT3 pos = g_BoxPos;
		InitItemWithHitBoxSingle(&g_ItemBox[i], MODEL_ITEM_BOX, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 3.0f, ITEM_SIZE, true, 10.0f, 10.0f, 10.0f, i, ITEM_TYPE_BOX);
	}

	// �}�b�v���ӂ̕ǂ̏�����
	for (int i = 0; i < CASTLE_WALL_NUMBER * 4; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (0 <= i && i < CASTLE_WALL_NUMBER) {
			pos = XMFLOAT3(MAP_TOP - 100.0f * i - 50.0f, 0.0f, MAP_RIGHT);
			InitItemWithHitBoxFromCsvSingle(&g_CastleWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}
		else if (CASTLE_WALL_NUMBER <= i && i < CASTLE_WALL_NUMBER * 2) {
			pos = XMFLOAT3(MAP_TOP - 100.0f * (i - CASTLE_WALL_NUMBER) - 50.0f, 0.0f, MAP_LEFT + 50.0f);
			InitItemWithHitBoxFromCsvSingle(&g_CastleWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}
		else if (CASTLE_WALL_NUMBER * 2 <= i && i < CASTLE_WALL_NUMBER * 3) {
			pos = XMFLOAT3(MAP_TOP , 0.0f, MAP_RIGHT - 100.0f * (i - CASTLE_WALL_NUMBER * 2));
			InitItemWithHitBoxFromCsvSingle(&g_CastleWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, XM_PI / 2, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}
		else if (CASTLE_WALL_NUMBER * 3 <= i && i < CASTLE_WALL_NUMBER * 4) {
			pos = XMFLOAT3(MAP_DOWN, 0.0f, MAP_RIGHT - 100.0f * (i - CASTLE_WALL_NUMBER * 3));
			InitItemWithHitBoxFromCsvSingle(&g_CastleWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, XM_PI / 2, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}

	}

	// �Q�[�g���ӂ̕ǂ̏�����
	for (int i = 0; i < GATE_WALL_NUMER + GATE_WALL_NUMER2 * 2; i++) {
		XMFLOAT3 pos = GATE_WALL_POS;
		if (0 <= i && i < GATE_WALL_NUMER) {
			pos.x -= 100.0f * i;
			InitItemWithHitBoxFromCsvSingle(&g_GateWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}
		else if (GATE_WALL_NUMER <= i && i < GATE_WALL_NUMER + GATE_WALL_NUMER2) {
			pos.x -= 100.0f * GATE_WALL_NUMER - 50.0f;
			pos.z -= 100.0f * (i - GATE_WALL_NUMER) + 50.0f;
			InitItemWithHitBoxFromCsvSingle(&g_GateWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, XM_PI / 2, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}
		else {
			pos.x -= 100.0f * GATE_WALL_NUMER - 50.0f;
			pos.z -= 100.0f * (2 + i - GATE_WALL_NUMER) + 50.0f;
			InitItemWithHitBoxFromCsvSingle(&g_GateWall[i], MODEL_ITEM_CASTLEWALL, true, pos, XMFLOAT3(0.0f, XM_PI / 2, 0.0f), 0.1f, ITEM_SIZE, true, DATA_CASTLE_WALL, i, ITEM_TYPE_CASTLE_WALL);
		}
	}





	//g_ItemBullet[ITEM_AMMO_TABLE].pos = g_ItemDecoration[ITEM_DECORATION_TABLE00].pos;
	//g_ItemHandgun[ITEM_HANDGUN_TABLE].pos = g_ItemDecoration[ITEM_DECORATION_TABLE00].pos;

	// �h�A�ŏ��͕�����
	ChangeItemUse(&g_ItemDoor[ITEM_HOUSE_DOOR_OPENED], false);

	// �R���g���[���[�ŏ���OFF���
	ChangeItemUse(&g_ItemController[ITEM_CONTROLLER_ON], false);
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
	
#ifdef _DEBUG
	PrintDebugProc("boxHit:%d\n", g_boxHit);
#endif // _DEBUG

	// �h�A�J�̌J��Ԃ��h�~
	if (g_CanOpenDoor == false) {
		g_OpenDoorCount++;
		if (g_OpenDoorCount > OPENDOOR_CD) {
			g_OpenDoorCount = 0;
			g_CanOpenDoor = true;
		}
	}

	// �h�A�J�̌J��Ԃ��h�~
	if (g_CanOpenController == false) {
		g_OpenControllerCount++;
		if (g_OpenControllerCount > OPENCONTROLLER_CD) {
			g_OpenControllerCount = 0;
			g_CanOpenController = true;
		}
	}

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

	// �}�b�v����̕ǂ̕`��
	for (int i = 0; i < CASTLE_WALL_NUMBER * 4; i++) {
		if (g_CastleWall[i].use == false) continue;
		DrawItemSingle(&g_CastleWall[i]);
	}

	// �R���g���[���[�̕`��
	for (int i = 0; i < ITEM_CONTROLLER_MAX; i++) {
		if (g_ItemController[i].use == false) continue;
		DrawItemSingle(&g_ItemController[i]);
	}

	// �Q�[�g���ӂ̕ǂ̕`��
	for (int i = 0; i < GATE_WALL_NUMER + GATE_WALL_NUMER2 * 2; i++) {
		if (g_GateWall[i].use == false) continue;
		DrawItemSingle(&g_GateWall[i]);
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

//�@�A�C�e���̃A�N�e�B�u��Ԃ��R���g���[������
void ChangeItemUse(ITEM* item, bool use) {
	MESHBOX* meshbox = GetMeshBox();
	item->use = use;
	meshbox[item->hitBoxIndex].use = use;
}

// �h�A�̑���
void OpenCloseDoor() {
	// ���݁@�h�A�͑���ł�����
	if (g_CanOpenDoor) {
		// ���h�A���߂Ă����ԂȂ�@�J����
		if (g_ItemDoor[ITEM_HOUSE_DOOR].use) {
			ChangeItemUse(&g_ItemDoor[ITEM_HOUSE_DOOR], false);
			ChangeItemUse(&g_ItemDoor[ITEM_HOUSE_DOOR_OPENED], true);
			g_CanOpenDoor = false;
		}
		else {
			ChangeItemUse(&g_ItemDoor[ITEM_HOUSE_DOOR], true);
			ChangeItemUse(&g_ItemDoor[ITEM_HOUSE_DOOR_OPENED], false);
			g_CanOpenDoor = false;
		}
	}
}

// �h�A�̑���
void OpenCloseController() {
	// ���݁@�R���g���[���[�͑���ł�����
	if (g_CanOpenController) {
		// �R���g���[���[����OFF��ԂȂ�@ON�ɂ���
		if (g_ItemController[ITEM_CONTROLLER_OFF].use) {
			ChangeItemUse(&g_ItemController[ITEM_CONTROLLER_OFF], false);
			ChangeItemUse(&g_ItemController[ITEM_CONTROLLER_ON], true);
			g_CanOpenController = false;
		}
		else {
			ChangeItemUse(&g_ItemController[ITEM_CONTROLLER_OFF], true);
			ChangeItemUse(&g_ItemController[ITEM_CONTROLLER_ON], false);
			g_CanOpenController = false;
		}
	}
}

bool BoxCheckWall(void) {
	// �{�b�N�X�ƕǂ̓����蔻��
	XMFLOAT3 housePos = HOUSE_POS;
	//XMFLOAT3 preBoxPos = g_ItemBox[ITEM_BOX_TEST].pos;
	//XMFLOAT3 preVPos[8];
	//for (int i = 0; i < 8; i++) {
	//	preVPos[i] = meshbox[g_ItemBox[ITEM_BOX_TEST].hitBoxIndex].vPos[i];
	//}
	housePos.x -= 20.0f;
	g_boxHit = false;
	if (CollisionBBXZ(g_ItemBox[ITEM_BOX_TEST].pos, 15.0f, 15.0f, housePos, 170.0f, 170.0f)) {
		return true;
		g_boxHit = true;
		//g_ItemBox[ITEM_BOX_TEST].pos = preBoxPos;
		//meshbox[g_ItemBox[ITEM_BOX_TEST].hitBoxIndex].pos = preBoxPos;
		//for (int i = 0; i < 8; i++) {
		//	meshbox[g_ItemBox[ITEM_BOX_TEST].hitBoxIndex].vPos[i] = preVPos[i];
		//}
	}
	return false;
}

bool CheckPlayerInHouse(void) {
	PLAYER* player = GetPlayer();
	XMFLOAT3 housePos = HOUSE_POS;
	housePos.x -= 20.0f;
	if (CollisionBBXZ(player->pos, 10.0f, 10.0f, housePos, 155.0f, 155.0f)) {
		return true;
	}
	return false;
}

XMFLOAT3 GetHousePos(void){
	return g_HousePos;
}

bool IsDoorOpened(void) {
	return g_ItemDoor[ITEM_HOUSE_DOOR_OPENED].use ? true : false;
}

bool IsControllerOpened(void) {
	return g_ItemController[ITEM_CONTROLLER_ON].use ? true : false;
}