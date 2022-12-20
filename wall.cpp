//=============================================================================
//
// エネミーモデル処理 [wall.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "wall.h"
#include "shadow.h"
#include "meshfield.h"
#include "meshwall.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_WALL			"data/MODEL/wall.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量


#define wall_OFFSET_Y		(0.0f)						// 地面に沈まないように

static MESHWALL g_MeshWall[MESHWALL_MAX];
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static WALL			g_Wall[MAX_WALL];				// WALL

static BOOL			g_Load = FALSE;





//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitWall(void)
{
	for (int i = 0; i < MAX_WALL; i++)
	{
		LoadModel(MODEL_WALL, &g_Wall[i].model);
		g_Wall[i].load = true;

		g_Wall[i].pos = XMFLOAT3(0.0f * 30.0f, wall_OFFSET_Y, 0.0f);
		g_Wall[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Wall[i].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);


		g_Wall[i].size = WALL_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Wall[0].model, &g_Wall[0].diffuse[0]);

		XMFLOAT3 pos = g_Wall[i].pos;
		pos.y -= (wall_OFFSET_Y - 0.1f);


		g_Wall[i].use = false;			// true:生きてる

	}

	for (int i = 0; i < MESHWALL_MAX; i++) {
		g_MeshWall[i].use = false;
	}


	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitWall(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_WALL; i++)
	{
		if (g_Wall[i].load)
		{
			UnloadModel(&g_Wall[i].model);
			g_Wall[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateWall(void)
{
	

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawWall(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_WALL; i++)
	{
		if (g_Wall[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Wall[i].scl.x, g_Wall[i].scl.y, g_Wall[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Wall[i].rot.x, g_Wall[i].rot.y + XM_PI, g_Wall[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Wall[i].pos.x, g_Wall[i].pos.y, g_Wall[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Wall[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Wall[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
WALL* GetWall()
{
	return &g_Wall[0];
}

void SetMeshWall(XMFLOAT3 pos,XMFLOAT3 rot,XMFLOAT4 diff,float width,float height) {
	for (int i = 0; i < MESHWALL_MAX; i++) {
		if (g_MeshWall[i].use == false) {
			InitMeshWall(pos, rot, diff, 1, 1, width, height);
			// 頂点座標の保存
			for (int nCntVtxY = 0; nCntVtxY < 2; nCntVtxY++)
			{
				for (int nCntVtxX = 0; nCntVtxX < 2; nCntVtxX++)
				{
					// 頂点座標の設定
					g_MeshWall[i].vPos[nCntVtxY*2+nCntVtxX].x = -0.5 * width + nCntVtxX * width;
					g_MeshWall[i].vPos[nCntVtxY*2+nCntVtxX].y = 1 * height - nCntVtxY * height;
					g_MeshWall[i].vPos[nCntVtxY*2+nCntVtxX].z = 0.0f;
				}
			}
			g_MeshWall[i].use = true;
		}
		return;
	}
}

MESHWALL* GetMeshWall() {
	return &g_MeshWall[0];
}