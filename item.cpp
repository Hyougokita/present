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
#define MODEL_ITEM_WINDOW						"data/MODEL/window.obj"

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ITEM_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ITEM_OFFSET_Y		(17.0f)						// エネミーの足元をあわせる

#define	AMMO_BOX_ADD_NUM	(20)						// 弾ボックスが増加する弾の数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
void DrawItemSingle(ITEM* item);
void InitItemWithHitBoxSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use, float hitboxW, float hitboxH, float hitboxD, int num, int type);
void InitItemSingle(ITEM* item, char* ModelName, bool load, XMFLOAT3 pos, XMFLOAT3 rot, float scl, float size, bool use);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ITEM			g_ItemBullet[ITEM_AMMO_MAX];				// AMMO
static ITEM			g_ItemDecoration[ITEM_DECORATION_TYPE_MAX];	// 飾り用アイテム
static ITEM			g_ItemHandgun[ITEM_HANDGUN_MAX];			// ピストル
static ITEM			g_ItemBox[ITEM_BOX_MAX];					// ボックス
static ITEM			g_ItemWindow[ITEM_WINDOW_MAX];

static BOOL			g_Load = FALSE;



// 飾り用アイテム関連
// アイテムのモデルデータ
static char* itemDecorationList[ITEM_DECORATION_TYPE_MAX] = {
	MODEL_ITEM_TABLE00,
	MODEL_ITEM_DOOR,
	MODEL_ITEM_HOUSE,
};
//　アイテムの位置
static XMFLOAT3 itemDecorationPositionList[ITEM_DECORATION_TYPE_MAX] = {
	XMFLOAT3(0.0f,0.0f,0.0f),
	XMFLOAT3(50.0f,0.0f,0.0f),
	XMFLOAT3(50.0f,0.0f,0.0f),
};
//	アイテムのスカラー
static float itemDecorationScaleList[ITEM_DECORATION_TYPE_MAX] = {
	 0.5f,
	 3.0f,
	 3.0f,
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{


	//	飾り用アイテムの初期化
	for (int i = 0; i < ITEM_DECORATION_TYPE_MAX; i++) {
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		InitItemSingle(&g_ItemDecoration[i], itemDecorationList[i], true, itemDecorationPositionList[i], XMFLOAT3(0.0f, 0.0f, 0.0f), itemDecorationScaleList[i], 4.0f, true);
	}
	g_ItemDecoration[ITEM_DECORATION_HOUSETEST].use = false;
	g_ItemDecoration[ITEM_DECORATION_DOORTEST].use = false;



	// AMMOの初期化
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

	//　ピストルの初期化
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

	// ボックスの初期化
	for (int i = 0; i < ITEM_BOX_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(-50.0f, 0.0f, 0.0f);
		InitItemWithHitBoxSingle(&g_ItemBox[i], MODEL_ITEM_BOX, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 3.0f, ITEM_SIZE, true, 10.0f, 10.0f, 10.0f, i, ITEM_TYPE_BOX);
	}

	// 窓の初期化
	for (int i = 0; i < ITEM_WINDOW_MAX; i++)
	{
		XMFLOAT3 pos = XMFLOAT3(-100.0f, 0.0f, 0.0f);
		InitItemWithHitBoxSingle(&g_ItemWindow[i], MODEL_ITEM_WINDOW, true, pos, XMFLOAT3(0.0f, 0.0f, 0.0f), 3.0f, ITEM_SIZE, true, 10.0f, 10.0f, 10.0f, i, ITEM_TYPE_WINDOW);
	}
	g_ItemWindow[ITEM_WINDOW_TEST].use = false;

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
// 更新処理
//=============================================================================
void UpdateItem(void)
{
#ifdef _DEBUG
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