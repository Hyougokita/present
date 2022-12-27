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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ITEM			"data/MODEL/item/ammo.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ITEM_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ITEM_OFFSET_Y		(17.0f)						// エネミーの足元をあわせる



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ITEM			g_Item[MAX_ITEM];				// エネミー

static BOOL			g_Load = FALSE;









//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{
	// 本体
	for (int i = 0; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM, &g_Item[i].model);
		g_Item[i].load = true;
		g_Item[i].pos = XMFLOAT3(-50.0f + i * 50.0f, ITEM_OFFSET_Y, -40.0f);
		g_Item[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Item[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_Item[i].size = ITEM_SIZE;	// 当たり判定の大きさ
		g_Item[i].use = true;
		g_Item[i].hitBoxIndex = SetMeshBox(g_Item[i].pos, g_Item[i].rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), 28.0f * g_Item[i].scl.x, 8.0f * g_Item[i].scl.y, 18.0f * g_Item[i].scl.z, i);

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);

		XMFLOAT3 pos = g_Item[i].pos;
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

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].load)
		{
			UnloadModel(&g_Item[i].model);
			g_Item[i].load = false;
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
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Item[i].scl.x, g_Item[i].scl.y, g_Item[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Item[i].rot.x, g_Item[i].rot.y + XM_PI, g_Item[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Item[i].pos.x, g_Item[i].pos.y, g_Item[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Item[i].mtxWorld, mtxWorld);

		DrawModel(&g_Item[i].model);
		
	}



	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ITEM *GetItem()
{
	return &g_Item[0];
}

//　指定されたアイテムを無効化する
void DestoryItem(int num) {
	g_Item[num].use = false;
}


