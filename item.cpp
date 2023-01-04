//=============================================================================
//
// エネミーモデル処理 [item.cpp]
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
// マクロ定義
//*****************************************************************************
#define	MODEL_ITEM_AMMO							"data/MODEL/item/ammo.obj"					// 弾薬ボックス
#define	MODEL_ITEM_HANDGUN						"data/MODEL/item/handgun_table.obj"			// 机上のピストル
#define	MODEL_ITEM_TABLE00						"data/MODEL/item/table00.obj"				// 机
#define	MODEL_ITEM_BOX							"data/MODEL/box.obj"						// 箱（移動させる　乗せる）
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

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ITEM_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ITEM_OFFSET_Y		(17.0f)						// エネミーの足元をあわせる

#define	AMMO_BOX_ADD_NUM	(20)						// 弾ボックスが増加する弾の数
#define HOUSE_POS			(XMFLOAT3(-50.0f,0.0f,100.0f))
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
void DrawItemSingle(ITEM* item);
void InitItemWithHitBoxSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, float hitboxW, float hitboxH, float hitboxD, int num, int type);
void InitItemSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use);
void InitItemWithHitBoxFromCsvSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, int dateNum, int num, int type);
void UninitItemSingle(ITEM* item);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ITEM			g_ItemBullet[ITEM_AMMO_MAX];				// AMMO
static ITEM			g_ItemDecoration[ITEM_DECORATION_TYPE_MAX];	// 飾り用アイテム
static ITEM			g_ItemHandgun[ITEM_HANDGUN_MAX];			// ピストル
static ITEM			g_ItemBox[ITEM_BOX_MAX];					// ボックス
static ITEM			g_ItemWindow[ITEM_WINDOW_MAX];				// 家のウィンド
static ITEM			g_ItemHouseWall[ITEM_WALL_MAX];				// 家の壁
static ITEM			g_ItemDoor[ITEM_HOUSE_DOOR_MAX];			// 家のドア
static ITEM			g_ItemTable[ITEM_TABLE_MAX];				// アイテムが置いてある机
#ifdef _DEBUG
static ITEM g_ItemTestAmmo[ITEM_TEST_AMMO_MAX];
#endif // _DEBUG


static BOOL			g_Load = FALSE;



// 飾り用アイテム関連
// アイテムのモデルデータ
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

//　アイテムの位置
static XMFLOAT3 itemDecorationPositionList[ITEM_DECORATION_TYPE_MAX] = {
	XMFLOAT3(50.0f,0.0f,0.0f),
	XMFLOAT3(50.0f,0.0f,0.0f),
	HOUSE_POS,
	HOUSE_POS,
};
//	アイテムのスカラー
static float itemDecorationScaleList[ITEM_DECORATION_TYPE_MAX] = {
	 3.0f,
	 3.0f,
	 1.0f,
	 1.0f,
};


// hitbox自動生成 wall
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
// 初期化処理
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
	// 家の壁の初期化
	for (int i = 0; i < ITEM_WALL_MAX; i++) {
		XMFLOAT3 pos = HOUSE_POS;
		InitItemWithHitBoxFromCsvSingle(&g_ItemHouseWall[i], itemHouseWallModelList[i], true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, 4.0f, true, 2 + i, i, ITEM_TYPE_WALL);
	}

	// 窓の初期化
	for (int i = 0; i < ITEM_WINDOW_MAX; i++)
	{
		XMFLOAT3 pos = HOUSE_POS;
		InitItemWithHitBoxFromCsvSingle(&g_ItemWindow[i], MODEL_ITEM_WINDOW, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, ITEM_SIZE, true, 1, i, ITEM_TYPE_WINDOW);
	}

	//　ドアの初期化
	for (int i = 0; i < ITEM_HOUSE_DOOR_MAX; i++) {
		XMFLOAT3 pos = HOUSE_POS;
		pos.x -= 90.0f;
		pos.z -= 8.5f;
		InitItemWithHitBoxFromCsvSingle(&g_ItemDoor[i], itemDoorModelList[i], true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, ITEM_SIZE, true, doorDataList[i], i, ITEM_TYPE_DOOR);
	}
	g_ItemDoor[ITEM_HOUSE_DOOR_OPENED].use = false;

	//	飾り用アイテムの初期化
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		InitItemSingle(&g_ItemDecoration[i], itemDecorationList[i], true, itemDecorationPositionList[i], XMFLOAT3(0.0f, 0.0f, 0.0f), itemDecorationScaleList[i], 4.0f, true);
	}
	g_ItemDecoration[ITEM_DECORATION_HOUSETEST].use = false;
	g_ItemDecoration[ITEM_DECORATION_DOORTEST].use = false;
	//g_ItemDecoration[ITEM_DECORATION_DOORTEST].pos = HOUSE_POS;



	// 机の初期化
	for (int i = 0; i < ITEM_TABLE_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		InitItemWithHitBoxFromCsvSingle(&g_ItemTable[i], MODEL_ITEM_TABLE00, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.5f, 5.0f, true, DATA_TABLE,i,ITEM_TYPE_TABLE);
	}

	// AMMOの初期化
	for (int i = 0; i < ITEM_AMMO_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (i == ITEM_AMMO_TABLE) {
			// 机上の弾ボックス
			pos = g_ItemTable[ITEM_TABLE].pos;
			pos.x += 14.0f;
			pos.y += 14.0f;

		}
		InitItemWithHitBoxSingle(&g_ItemBullet[i], MODEL_ITEM_AMMO, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.5f, ITEM_SIZE, true, 28.0f, 8.0f, 18.0f, i, ITEM_AMMO_BOX);

	}

	//　ピストルの初期化
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// 机の上ピストル
		if (i == ITEM_HANDGUN_TABLE) {
			pos = g_ItemTable[ITEM_TABLE].pos;
			pos.x -= 14.0f;
			pos.y += 14.0f;
		}
		InitItemWithHitBoxSingle(&g_ItemHandgun[i], MODEL_ITEM_HANDGUN, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 0.2f, ITEM_SIZE, true, 60.0f, 16.0f, 56.0f, i, ITEM_HAND_GUN);
	}

	// ボックスの初期化
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
// 終了処理
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	// 壁の終了
	for (int i = 0; i < ITEM_WALL_MAX; i++) {
		UninitItemSingle(&g_ItemHouseWall[i]);
	}

	// 窓の終了
	for (int i = 0; i < ITEM_WINDOW_MAX; i++) {
		UninitItemSingle(&g_ItemWindow[i]);
	}

	// ドアの終了
	for (int i = 0; i < ITEM_HOUSE_DOOR_MAX; i++) {
		UninitItemSingle(&g_ItemDoor[i]);
	}

	// 飾りの終了
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		UninitItemSingle(&g_ItemDecoration[i]);
	}

	// 机の終了
	for (int i = 0; i < ITEM_TABLE_MAX; i++) {
		UninitItemSingle(&g_ItemTable[i]);
	}

	// AMMOの終了
	for (int i = 0; i < ITEM_AMMO_MAX; i++) {
		UninitItemSingle(&g_ItemBullet[i]);
	}

	// ピストルの終了
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++) {
		UninitItemSingle(&g_ItemHandgun[i]);
	}

	// ボックスの終了
	for (int i = 0; i < ITEM_BOX_MAX; i++) {
		UninitItemSingle(&g_ItemBox[i]);
	}

	g_Load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateItem(void)
{
#ifdef _DEBUG
	// ドアを回転させる
	g_ItemDecoration[ITEM_DECORATION_DOORTEST].rot.y += 0.01f;
#endif // _DEBUG

}



//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
#ifdef _DEBUG
	for (int i = 0; i < ITEM_TEST_AMMO_MAX; i++) {
		if (g_ItemTestAmmo[i].use == false)	continue;
		DrawItemSingle(&g_ItemTestAmmo[i]);
	}
#endif // _DEBUG

	// 家の壁の描画
	for (int i = 0; i < ITEM_WALL_MAX; i++) {
		if (g_ItemHouseWall[i].use == false) continue;
		DrawItemSingle(&g_ItemHouseWall[i]);
	}

	//　ドアの描画
	for (int i = 0; i < ITEM_HOUSE_DOOR_MAX; i++) {
		if (g_ItemDoor[i].use == false) continue;
		DrawItemSingle(&g_ItemDoor[i]);
	}

	// 机の描画
	for (int i = 0; i < ITEM_TABLE_MAX; i++) {
		if (g_ItemTable[i].use == false) continue;
		DrawItemSingle(&g_ItemTable[i]);
	}

	// 弾ボックスの描画
	for (int i = 0; i < ITEM_AMMO_MAX; i++)
	{
		if (g_ItemBullet[i].use == false) continue;
		DrawItemSingle(&g_ItemBullet[i]);
	}

	// ピストルの描画
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		if (g_ItemHandgun[i].use == false) continue;
		DrawItemSingle(&g_ItemHandgun[i]);
	}

	// ボックスの描画
	for (int i = 0; i < ITEM_HANDGUN_MAX; i++)
	{
		if (g_ItemBox[i].use == false) continue;
		DrawItemSingle(&g_ItemBox[i]);
	}

	// 窓の描画
	for (int i = 0; i < ITEM_WINDOW_MAX; i++)
	{
		if (g_ItemWindow[i].use == false) continue;
		DrawItemSingle(&g_ItemWindow[i]);
	}

	//　飾り用アイテムの描画
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		if (g_ItemDecoration[i].use == false) continue;
		DrawItemSingle(&g_ItemDecoration[i]);
	}



	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ITEM *GetItemBullet()
{
	return &g_ItemBullet[0];
}

ITEM* GetItemBox() {
	return &g_ItemBox[0];
}

//　指定されたアイテムを無効化する
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

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(item->scl.x, item->scl.y, item->scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(item->rot.x, item->rot.y + XM_PI, item->rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// 移動を反映
	mtxTranslate = XMMatrixTranslation(item->pos.x, item->pos.y, item->pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&item->mtxWorld, mtxWorld);

	DrawModel(&item->model);
}

// ヒットボックス付きのアイテムの初期化
void InitItemWithHitBoxSingle(ITEM* item, char* ModelName,bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use,float hitboxW,float hitboxH,float hitboxD, int num, int type) {
	LoadModel(ModelName, &item->model);
	item->load = load;
	item->pos = pos;
	item->rot = rot;
	item->scl = XMFLOAT3(scl, scl, scl);

	item->size = size;	// 当たり判定の大きさ
	item->use = use;
	item->hitBoxIndex = SetMeshBox(pos, rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), hitboxW * scl, hitboxH * scl, hitboxD * scl, num, type);

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&item->model, &item->diffuse[0]);
}

void InitItemWithHitBoxFromCsvSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, int dateNum, int num, int type) {
	LoadModel(ModelName, &item->model);
	item->load = load;
	item->pos = pos;
	item->rot = rot;
	item->scl = XMFLOAT3(scl, scl, scl);

	item->size = size;	// 当たり判定の大きさ
	item->use = use;
	item->hitBoxIndex = SetMeshBoxFromData(pos, rot, scl, num, type, dateNum);

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&item->model, &item->diffuse[0]);
}

//　ヒットボックスなしのアイテムの初期化
void InitItemSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use) {
	LoadModel(ModelName, &item->model);
	item->load = load;
	item->pos = pos;
	item->rot = rot;
	item->scl = XMFLOAT3(scl, scl, scl);

	item->size = size;	// 当たり判定の大きさ
	item->use = use;
	item->hitBoxIndex = -99;

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&item->model, &item->diffuse[0]);
}

void UninitItemSingle(ITEM* item) {
	if (item->load)
	{
		UnloadModel(&item->model);
		item->load = false;
	}
}