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
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ITEM			"data/MODEL/item/ammo.obj"		// 読み込むモデル名

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
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ITEM			g_ItemBullet[MAX_ITEMBULLET];				// エネミー

static BOOL			g_Load = FALSE;









//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{
	// 本体
	for (int i = 0; i < MAX_ITEMBULLET; i++)
	{
		LoadModel(MODEL_ITEM, &g_ItemBullet[i].model);
		g_ItemBullet[i].load = true;
		g_ItemBullet[i].pos = XMFLOAT3(-50.0f + i * 50.0f, ITEM_OFFSET_Y, -40.0f);
		g_ItemBullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ItemBullet[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_ItemBullet[i].size = ITEM_SIZE;	// 当たり判定の大きさ
		g_ItemBullet[i].use = true;
		g_ItemBullet[i].hitBoxIndex = SetMeshBox(g_ItemBullet[i].pos, g_ItemBullet[i].rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), 28.0f * g_ItemBullet[i].scl.x, 8.0f * g_ItemBullet[i].scl.y, 18.0f * g_ItemBullet[i].scl.z, i, ITEM_AMMO_BOX);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_ItemBullet[0].model, &g_ItemBullet[0].diffuse[0]);

		XMFLOAT3 pos = g_ItemBullet[i].pos;
		pos.y -= (ITEM_OFFSET_Y - 0.1f);
		
	}

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
	
}



//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
{


	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// 弾ボックスの描画
	for (int i = 0; i < MAX_ITEMBULLET; i++)
	{
		if (g_ItemBullet[i].use == false) continue;
		DrawItemSingle(&g_ItemBullet[i]);
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

//　指定されたアイテムを無効化する
void DestoryItem(int num, char itemType) {
	if (itemType == ITEM_AMMO_BOX) {
		g_ItemBullet[num].use = false;
		AddSubMagazine(AMMO_BOX_ADD_NUM);
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
