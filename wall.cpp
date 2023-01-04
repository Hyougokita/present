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
#include "item.h"
// openfileのwarningの防止
#pragma warning(disable : 4996)
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_WALL			"data/MODEL/wall.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量


#define wall_OFFSET_Y		(0.0f)						// 地面に沈まないように

static MESHWALL g_MeshWall[MESHWALL_MAX];
static MESHBOX g_MeshBox[MESHBOX_MAX];

// 頂点データの読み込み
#define FILE_NAME "data/hitbox.csv"





static float vPosList[DATA_MAX][24];
static char  name[DATA_MAX][40];
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadVPos(void);

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
	LoadVPos();
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

	for (int i = 0; i < MESHBOX_MAX; i++) {
		g_MeshBox[i].use = false;
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

// メッシュウォール当たり判定用ポリゴン
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


					//	拡大縮小　回転　移動の変換の反映
					XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
					// ワールドマトリックスの初期化
					mtxWorld = XMMatrixIdentity();

					// スケールを反映
					mtxScl = XMMatrixScaling(1.0f, 1.0f, 1.0f);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

					// 回転を反映
					mtxRot = XMMatrixRotationRollPitchYaw(rot.x, rot.y + XM_PI, rot.z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

					// 移動を反映
					mtxTranslate = XMMatrixTranslation(pos.x, pos.y, pos.z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

					//　位置を行列に保存する
					XMMATRIX vPos;
					for (int m = 0; m < 4; m++) {
						for (int n = 0; n < 4; n++) {
							vPos.r[m].m128_f32[n] = 0;
						}
					}
					vPos.r[0].m128_f32[0] = g_MeshWall[i].vPos[nCntVtxY * 2 + nCntVtxX].x;
					vPos.r[0].m128_f32[1] = g_MeshWall[i].vPos[nCntVtxY * 2 + nCntVtxX].y;
					vPos.r[0].m128_f32[2] = g_MeshWall[i].vPos[nCntVtxY * 2 + nCntVtxX].z;
					vPos.r[0].m128_f32[3] = 1.0f;

					// 変換後の座標を計算する
					XMMATRIX result = XMMatrixMultiply(vPos, mtxWorld);

					// 変換したあとの座標を保存する
					g_MeshWall[i].vPos[nCntVtxY * 2 + nCntVtxX].x = result.r[0].m128_f32[0];
					g_MeshWall[i].vPos[nCntVtxY * 2 + nCntVtxX].y = result.r[0].m128_f32[1];
					g_MeshWall[i].vPos[nCntVtxY * 2 + nCntVtxX].z = result.r[0].m128_f32[2];

				}
			}
			g_MeshWall[i].use = true;
			return;
		}
	}
}

MESHWALL* GetMeshWall() {
	return &g_MeshWall[0];
}

// メッシュウォール当たり判定用ポリゴン
int SetMeshBox(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 diff, float width, float height,float depth,char itemNum, char itemType) {
	for (int i = 0; i < MESHBOX_MAX; i++) {
		if (g_MeshBox[i].use == false) {


			//前面の初期化
			XMFLOAT3 frontPos = pos;
			frontPos.z -= 0.5 * depth;

			// 頂点座標の保存
			for (int nCntVtxY = 0; nCntVtxY < 2; nCntVtxY++)
			{
				for (int nCntVtxX = 0; nCntVtxX < 2; nCntVtxX++)
				{
					// 頂点座標の設定
					g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].x = -0.5 * width + nCntVtxX * width;
					g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].y = 1 * height - nCntVtxY * height;
					g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].z = 0.0f;


					//	拡大縮小　回転　移動の変換の反映
					XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
					// ワールドマトリックスの初期化
					mtxWorld = XMMatrixIdentity();

					// スケールを反映
					mtxScl = XMMatrixScaling(1.0f, 1.0f, 1.0f);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

					// 回転を反映
					mtxRot = XMMatrixRotationRollPitchYaw(rot.x, rot.y + XM_PI, rot.z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

					// 移動を反映
					mtxTranslate = XMMatrixTranslation(pos.x, pos.y, frontPos.z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

					//　位置を行列に保存する
					XMMATRIX vPos;
					for (int m = 0; m < 4; m++) {
						for (int n = 0; n < 4; n++) {
							vPos.r[m].m128_f32[n] = 0;
						}
					}
					vPos.r[0].m128_f32[0] = g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].x;
					vPos.r[0].m128_f32[1] = g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].y;
					vPos.r[0].m128_f32[2] = g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].z;
					vPos.r[0].m128_f32[3] = 1.0f;

					// 変換後の座標を計算する
					XMMATRIX result = XMMatrixMultiply(vPos, mtxWorld);

					// 変換したあとの座標を保存する
					g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].x = result.r[0].m128_f32[0];
					g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].y = result.r[0].m128_f32[1];
					g_MeshBox[i].vPos[nCntVtxY * 2 + nCntVtxX].z = result.r[0].m128_f32[2];

				}
			}



			//後ろの初期化
			XMFLOAT3 backPos = pos;
			backPos.z += 0.5 * depth;

			XMFLOAT3 backrot = rot;
			backrot.y += XM_PI;

			// 頂点座標の保存
			for (int nCntVtxY = 0; nCntVtxY < 2; nCntVtxY++)
			{
				for (int nCntVtxX = 0; nCntVtxX < 2; nCntVtxX++)
				{
					// 頂点座標の設定
					g_MeshBox[i].vPos[4+nCntVtxY * 2 + nCntVtxX].x = -0.5 * width + nCntVtxX * width;
					g_MeshBox[i].vPos[4+nCntVtxY * 2 + nCntVtxX].y = 1 * height - nCntVtxY * height;
					g_MeshBox[i].vPos[4+nCntVtxY * 2 + nCntVtxX].z = 0.0f;


					//	拡大縮小　回転　移動の変換の反映
					XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
					// ワールドマトリックスの初期化
					mtxWorld = XMMatrixIdentity();

					// スケールを反映
					mtxScl = XMMatrixScaling(1.0f, 1.0f, 1.0f);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

					// 回転を反映
					mtxRot = XMMatrixRotationRollPitchYaw(rot.x, backrot.y + XM_PI, rot.z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

					// 移動を反映
					mtxTranslate = XMMatrixTranslation(pos.x, pos.y, backPos.z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

					//　位置を行列に保存する
					XMMATRIX vPos;
					for (int m = 0; m < 4; m++) {
						for (int n = 0; n < 4; n++) {
							vPos.r[m].m128_f32[n] = 0;
						}
					}
					vPos.r[0].m128_f32[0] = g_MeshBox[i].vPos[4 + nCntVtxY * 2 + nCntVtxX].x;
					vPos.r[0].m128_f32[1] = g_MeshBox[i].vPos[4 + nCntVtxY * 2 + nCntVtxX].y;
					vPos.r[0].m128_f32[2] = g_MeshBox[i].vPos[4 + nCntVtxY * 2 + nCntVtxX].z;
					vPos.r[0].m128_f32[3] = 1.0f;

					// 変換後の座標を計算する
					XMMATRIX result = XMMatrixMultiply(vPos, mtxWorld);

					// 変換したあとの座標を保存する
					g_MeshBox[i].vPos[4 + nCntVtxY * 2 + nCntVtxX].x = result.r[0].m128_f32[0];
					g_MeshBox[i].vPos[4 + nCntVtxY * 2 + nCntVtxX].y = result.r[0].m128_f32[1];
					g_MeshBox[i].vPos[4 + nCntVtxY * 2 + nCntVtxX].z = result.r[0].m128_f32[2];

				}
			}
#ifdef _DEBUG
			//可視化部分
			// left
			XMFLOAT3 leftPos = pos;
			leftPos.x -= 0.5 * width;
			XMFLOAT3 leftRot = rot;
			leftRot.y = XM_PI / 2;
			InitMeshWall(leftPos, leftRot, diff, 1, 1, depth, height);

			// right
			XMFLOAT3 rightPos = pos;
			rightPos.x += 0.5 * width;
			XMFLOAT3 rightRot = rot;
			rightRot.y = -XM_PI / 2;
			InitMeshWall(rightPos, rightRot, diff, 1, 1, depth, height);

			// front
			InitMeshWall(frontPos, rot, diff, 1, 1, width, height);

			// back
			InitMeshWall(backPos, backrot, diff, 1, 1, width, height);

			// up
			XMFLOAT3 upPos = pos;
			pos.y += 0.5f * height;
			//InitMeshField(upPos, XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 1, width, depth);
#endif // DEBUG
			g_MeshBox[i].use = true;
			//　HITBOXが対応しているアイテムを記録する
			g_MeshBox[i].itemNum = itemNum;
			g_MeshBox[i].itemType = itemType;
			// 位置を記録する
			g_MeshBox[i].pos = pos;
			return i;
		}
	}
}


MESHBOX* GetMeshBox(void) {
	return &g_MeshBox[0];
}

// 指定されたHITBOXを無効化する
void DestoryMeshBox(int num) {
	g_MeshBox[num].use = false;
	DestoryItem(g_MeshBox[num].itemNum,g_MeshBox[num].itemType);
}

// CSVファイルから頂点データをロードする
void LoadVPos(void) {
	//行数のカウント
	int namecount = 0;
	//一行のデータ数のカウント
	int count = 0;
	
	FILE* fp;
	fp = fopen(FILE_NAME, "r");
	if (fp == NULL) {
		fprintf(stderr, "failed\n");
	}

	//一行のbuffer
	char row[300];
	char* token;
	while (fgets(row, 300, fp) != NULL) {
		// 名前の取得
		token = strtok(row, ",");
		strcpy(&name[namecount][0], token);

		while (token != NULL) {
			token = strtok(NULL, ",");
			if (token == NULL) {
				count = 0;
				break;
			}
			// 頂点データの取得
			vPosList[namecount][count] = atof(token);
			count++;
		}
		//改行
		namecount++;
	}
	fclose(fp);
}

float selfabs(float number) {
	return number < 0 ? -number : number;
}

int SetMeshBoxFromData(XMFLOAT3 pos, XMFLOAT3 rot, float scl,int itemNum, int itemType,int dataNum) {
	for (int i = 0; i < MESHBOX_MAX; i++) {
		if (g_MeshBox[i].use == false) {
			// 頂点データをlistから登録する
			for (int m = 0; m < 8; m++) {
				g_MeshBox[i].vPos[m].x = vPosList[dataNum][m * 3] * 10.0f;
				g_MeshBox[i].vPos[m].y = vPosList[dataNum][m * 3 + 1] * 10.0f;
				g_MeshBox[i].vPos[m].z = vPosList[dataNum][m * 3 + 2] * 10.0f;
			}

			//	拡大縮小　回転　移動の変換の反映
			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(scl, scl, scl);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(rot.x, rot.y + XM_PI, rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(pos.x, pos.y, pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// 頂点ごと変換させる
			for (int j = 0; j < 8; j++) {
				//　位置を行列に保存する
				XMMATRIX vPos;
				for (int m = 0; m < 4; m++) {
					for (int n = 0; n < 4; n++) {
						vPos.r[m].m128_f32[n] = 0;
					}
				}
				vPos.r[0].m128_f32[0] = g_MeshBox[i].vPos[j].x;
				vPos.r[0].m128_f32[1] = g_MeshBox[i].vPos[j].y;
				vPos.r[0].m128_f32[2] = g_MeshBox[i].vPos[j].z;
				vPos.r[0].m128_f32[3] = 1.0f;

				// 変換後の座標を計算する
				XMMATRIX result = XMMatrixMultiply(vPos, mtxWorld);

				// 変換したあとの座標を保存する
				g_MeshBox[i].vPos[j].x = result.r[0].m128_f32[0];
				g_MeshBox[i].vPos[j].y = result.r[0].m128_f32[1];
				g_MeshBox[i].vPos[j].z = result.r[0].m128_f32[2];
			}
			float width = selfabs(g_MeshBox[i].vPos[0].x - g_MeshBox[i].vPos[1].x);
			float height = selfabs(g_MeshBox[i].vPos[0].y - g_MeshBox[i].vPos[2].y);
			float depth = selfabs(g_MeshBox[i].vPos[0].z - g_MeshBox[i].vPos[5].z);
			//可視化部分
			//front
			//XMFLOAT3 frontPos = pos;
			//frontPos.z =g_MeshBox[i].vPos[4].z;
			//InitMeshWall(frontPos, rot, XMFLOAT4(1.0f,1.0f,1.0f,0.5f), 1, 1, width, height);
			//XMFLOAT3 rightPos = pos;
			//rightPos.x = g_MeshBox[i].vPos[4].x;
			//rightPos.z = g_MeshBox[i].vPos[4].z + selfabs(g_MeshBox[i].vPos[4].z - g_MeshBox[i].vPos[1].z) * 0.5f;
			////right 
			//InitMeshWall(rightPos, XMFLOAT3(0.0f,-XM_PI / 2,0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), 1, 1, depth, height);

			g_MeshBox[i].pos.x = g_MeshBox[i].vPos[0].x + width * 0.5f;
			g_MeshBox[i].pos.y = g_MeshBox[i].vPos[0].y - height * 0.5f;
			g_MeshBox[i].pos.z = g_MeshBox[i].vPos[0].z - depth * 0.5f;
			g_MeshBox[i].use = true;
			g_MeshBox[i].itemNum = itemNum;
			g_MeshBox[i].itemType = itemType;
			return i;
		}

	}
}