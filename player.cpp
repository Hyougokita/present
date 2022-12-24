//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "collision.h"
#include "wall.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DEBUG

#define	MODEL_PLAYER		"data/MODEL/cone.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS	"data/MODEL/torus.obj"			// 読み込むモデル名

// プレーヤーのモデル名
#define MODE_BERA_BODY			"data/MODEL/bear_body.obj"
#define MODE_BERA_RIGHT_HAND	"data/MODEL/bear_righthand.obj"
#define MODE_BERA_LEFT_HAND		"data/MODEL/bear_lefthand.obj"
#define MODE_BEAR_HEAD			"data/MODEL/bear_head.obj"
#define MODE_BEAR_RIGHT_LEG		"data/MODEL/bear_rightleg.obj"
#define MODE_BEAR_LEFT_LEG		"data/MODEL/bear_leftleg.obj"

//	武器のモデル名
#define MODE_HAND_GUN			"data/MODEL/handgun.obj"

char* playerModelPath[5] =
{ MODE_BERA_RIGHT_HAND,
	MODE_BERA_LEFT_HAND,
	MODE_BEAR_HEAD,
	MODE_BEAR_LEFT_LEG,
	MODE_BEAR_RIGHT_LEG,
};

char* weaponModelPath[1] = {
	MODE_HAND_GUN
};

enum PLAYER_PART_NAME
{
	PLAYER_RIGHT_HAND,
	PLAYER_LEFT_HAND,
	PLAYER_HEAD,
	PLAYER_LEFT_LEG,
	PLAYER_RIGHT_LEG
};


#define MODE_BEAR_RIGHT_LEG2	"data/MODEL/bear_rightleg2.obj"
#define MODE_BEAR_LEFT_LEG2		"data/MODEL/bear_leftleg2.obj"

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(17.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(6)								// プレイヤーのパーツの数
#define PLAYER_WEAPONS_MAX	(1)								// プレイヤーのパーツの数

#define DISTANCE_OF_RAYCAST_PLAYER  (20.0f)

XMFLOAT3 norScl = XMFLOAT3(0.4f, 0.4f, 0.4f);
XMFLOAT3 norScl2 = XMFLOAT3(1.0f, 1.0f, 1.0f);

//　初めてマウスロードする
bool g_FirstMouseLoad = false;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL CheckTest();
BOOL CheckTestWall();
void SenkeihokanPlayer(PLAYER* player, int type);		//	線形補間計算
void DrawPlayerSingle(PLAYER* player);					//	プレーヤー単体の描画

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static PLAYER		g_Weapon[PLAYER_WEAPONS_MAX];	//	プレーヤーの武器用

static float		roty = 0.0f;

static LIGHT		g_Light;

bool g_isMove = false;


enum PLAYER_HOKAN_TYPE
{
	FULLOUT,
	MOVE_BY_KEYBOARD,
};

#ifdef DEBUG
	BOOL isHitWall = false;
	BOOL bCheckHitWall = false;

	XMFLOAT3 vPosList[8] = {
		XMFLOAT3(-15.0f, 0.0f, 5.0f),
		XMFLOAT3(15.0f, 0.0f, 5.0f),
		XMFLOAT3(-15.0f, 10.0f, 5.0f),
		XMFLOAT3(15.0f, 10.0f, 5.0f),

		XMFLOAT3(-15.0f, 0.0f, -5.0f),
		XMFLOAT3(15.0f, 0.0f, -5.0f),
		XMFLOAT3(-15.0f, 10.0f, -5.0f),
		XMFLOAT3(15.0f, 10.0f, -5.0f),
	};

	static XMFLOAT3 vPos0 = XMFLOAT3(-15.0f, 0.0f, 5.0f);
	static XMFLOAT3 vPos1 = XMFLOAT3(15.0f, 0.0f, 5.0f);
	static XMFLOAT3 vPos2 = XMFLOAT3(-15.0f, 10.0f, 5.0f);
	static XMFLOAT3 vPos3 = XMFLOAT3(15.0f, 10.0f, 5.0f);
	
	static XMFLOAT3 vPos4 = XMFLOAT3(-15.0f, 0.0f, -5.0f);
	static XMFLOAT3 vPos5 = XMFLOAT3(15.0f, 0.0f, -5.0f);
	static XMFLOAT3 vPos6 = XMFLOAT3(-15.0f, 10.0f, -5.0f);
	static XMFLOAT3 vPos7 = XMFLOAT3(15.0f, 10.0f, -5.0f);
	static HITBOX testHitBox;

#endif // DEBUG




	// プレイヤーの階層アニメーションデータ
	// プレイヤーの頭を左右に動かしているアニメデータ
	static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_right_leg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f), norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_left_leg[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(0.0f,  -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), norScl2, 15 },
		{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },
		{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    norScl2, 15 },

	};

	static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    norScl2, 30 },

	};

	static INTERPOLATION_DATA move_tbl_handgun[] = {	// pos, rot, scl, frame
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         norScl2, 15 },
		{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		   norScl2, 30 },

	};



	//タイトルのアニメーシ

	static INTERPOLATION_DATA* g_MoveTblAdr[] =
	{
		move_tbl_right,
		move_tbl_left,
		move_tbl_head,
		move_tbl_left_leg,
		move_tbl_right_leg,
		move_tbl_right

	};







//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	WALL* wall = GetWall();
	XMFLOAT3 wallScale = wall[0].scl;
	for (int i = 0; i < 8; i++) {
		
	}

	g_Player.load = TRUE;
	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = XMFLOAT3(-10.0f, PLAYER_OFFSET_Y+50.0f, -200.0f);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.spdValue = VALUE_MOVE;	// 移動の基礎値
	g_Player.font = XMFLOAT3(0.0f,0.0f,0.0f);
	g_Player.use = TRUE;			// true:生きてる
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	// ジャンプ用
	g_Player.jumpType = JUMP_NONE;
	g_Player.jumpCnt = 0;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// キーを押した時のプレイヤーの向き
	roty = 0.0f;

	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる


	// 階層アニメーションの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		LoadModel(MODE_BERA_BODY, &g_Player.model);
		g_Player.load = true;

		g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
		g_Player.rot = { 0.0f, 0.0f, 0.0f };
		g_Player.scl = XMFLOAT3(0.2f, 0.2f, 0.2f);

		g_Player.spd = 0.0f;			// 移動スピードクリア
		g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

		g_Player.use = true;

		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Player.pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号



		// 階層アニメーション用の初期化処理
		g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる


		LoadModel(MODE_BERA_RIGHT_HAND, &g_Parts[RIGHT_HAND].model);
		LoadModel(MODE_BERA_LEFT_HAND, &g_Parts[LEFT_HAND].model);
		LoadModel(MODE_BEAR_HEAD, &g_Parts[HEAD].model);
		LoadModel(MODE_BEAR_RIGHT_LEG, &g_Parts[RIGHT_LEG].model);
		LoadModel(MODE_BEAR_LEFT_LEG, &g_Parts[LEFT_LEG].model);
		LoadModel(MODE_HAND_GUN, &g_Parts[5].model);
		// パーツの初期化
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			g_Parts[i].use = true;
			//LoadModel(MODEL_PLAYER_PARTS, &g_Parts[i].model);

			// 位置・回転・スケールの初期設定
			g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
		//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
		//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

			// 階層アニメーション用のメンバー変数の初期化
			g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[i].tblNo = i;			// 再生する行動データテーブルNoをセット
			g_Parts[i].tblMax = 3;			// 再生する行動データテーブルのレコード数をセット

			// パーツの読み込みはまだしていない
			g_Parts[i].load = 1;
		}

		//　プレーヤー武器の初期化
		for (int i = 0; i < PLAYER_WEAPONS_MAX; i++) {
			LoadModel(weaponModelPath[i], &g_Weapon[i].model); // ウェポンモデルをロードする
			g_Weapon[i].parent = &g_Parts[RIGHT_HAND];	// ウェポンを右手で持つ

			g_Weapon[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Weapon[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Weapon[i].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Weapon[i].use = true;
			g_Weapon[i].load = true;
		}

		//fpsなので　頭が邪魔　falseにする
		g_Parts[PLAYER_HEAD].use = false;
	}



	// クォータニオンの初期化
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	if (g_FirstMouseLoad == false) {
		//GetCursorPos(&lpPoint);
		//g_MousePos.x = (float)lpPoint.x;
		//g_MousePos.y = (float)lpPoint.y;
		//g_FirstMouseLoad = true;

	}


	CAMERA *cam = GetCamera();
	g_isMove = false;
	g_Player.spd *= 0.9f;

#ifdef DEBUG
	if (CheckTest()) {
		isHitWall = true;
	}
	else {
		isHitWall = false;
	}

	if (CheckTestWall()) {
		bCheckHitWall = true;
	}
	else
	{
		bCheckHitWall = false;
	}
	
#endif // DEBUG



	// 移動処理
	if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.x -= g_Player.spd;
		roty = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.x += g_Player.spd;
		roty = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.z += g_Player.spd;
		roty = XM_PI;
	}
	if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
	{
		g_isMove = true;
		g_Player.spd = g_Player.spdValue;
		//g_Player.pos.z -= g_Player.spd;
		roty = 0.0f;
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}
#endif


	{	// 押した方向にプレイヤーを移動させる
		// 押した方向にプレイヤーを向かせている所
		
		g_Player.font.y = roty + cam->rot.y;

		g_Player.prePos = g_Player.pos;

		g_Player.pos.x += sinf(g_Player.font.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.font.y) * g_Player.spd;

		if (bCheckHitWall) {
			g_Player.pos = g_Player.prePos;
		}
	}





	// 弾発射処理
	if (IsMouseLeftTriggered())
	{
		SetBullet(g_Player.pos, g_Player.font);
	}

	// 加速の処理
	g_Player.spdValue = VALUE_MOVE;
	if (GetKeyboardPress(DIK_LSHIFT)) {
		g_Player.spdValue = VALUE_MOVE * 2;
	}

	// ジャンプ処理
	if(GetKeyboardPress(DIK_SPACE)){
		// ジャンプしていない場合
		if (g_Player.jumpType == JUMP_NONE) {
			//　普通のジャンプ
			g_Player.jumpType = JUMP_NORMAL;
		}
	}

	// 普通のジャンプの場合
	if (g_Player.jumpType == JUMP_NORMAL) {
		g_Player.jumpCnt++;
		g_Player.pos.y += (JUMP_POWER - g_Player.jumpCnt * 0.3f);
		//g_Player.pos.y += 20.0f;
		if (g_Player.jumpCnt >= PLAYER_JUMP_COUNT_MAX) {
			g_Player.jumpCnt = 0;
			g_Player.jumpType = JUMP_NONE;
		}
	}

	//　重力より落下する
	g_Player.pos.y -= G;

	if (g_Player.pos.y <= PLAYER_OFFSET_Y) {
		g_Player.pos.y = PLAYER_OFFSET_Y;
	}

	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	bool ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		//g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		//g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		SenkeihokanPlayer(&g_Parts[i], MOVE_BY_KEYBOARD);
	}




	// ポイントライトのテスト
	{
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}



	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;


	g_Player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&g_Player.Quaternion, quat);




#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y);
	PrintDebugProc("isHitWall: %d\n", isHitWall);
	PrintDebugProc("bCheckHitWall: %d\n", bCheckHitWall);
	PrintDebugProc("font.y:%f\n",g_Player.font.y);
	//PrintDebugProc("Mouse Pos: X:%f,Y:%f", g_MousePos.x, g_MousePos.y);
	PrintDebugProc("DisplaySize:%d\n", GetSystemMetrics(SM_CXSCREEN));
	PrintDebugProc("Player Jump Flag:%d\n", g_Player.jumpType);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// クォータニオンを反映
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// 縁取りの設定
	//SetFuchi(1);

	// モデル描画
	DrawModel(&g_Player.model);



	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		DrawPlayerSingle(&g_Parts[i]);

	}

	//SetFuchi(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

//　プレイヤーと壁の当たり判定
BOOL CheckHitWall() {
	//　プレイヤーの移動方向へ射線を作る

	return false;
}

BOOL CheckTest() {
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	
	XMFLOAT3 startPos = g_Player.pos;	//　始点をプレイヤーの位置にする
	XMFLOAT3 endPos = g_Player.pos;		
	endPos.z += 10.0f;					//	テストなので　終了位置をプレイヤーの位置より10.0f奥の位置にする
	
	isHit = RayCast(vPos0, vPos1, vPos2, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}
	isHit = RayCast(vPos1, vPos2, vPos3, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}

	isHit = RayCast(vPos4, vPos5, vPos6, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}
	isHit = RayCast(vPos5, vPos6, vPos7, startPos, endPos, &HitPosition, &Normal);
	if (isHit) {
		return TRUE;
	}

	return FALSE;
}

BOOL CheckTestWall() {
	MESHWALL* meshwall = GetMeshWall();
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）

	XMFLOAT3 startPos = g_Player.pos;	//　始点をプレイヤーの位置にする
	XMFLOAT3 endPos = g_Player.pos;
	//endPos.z -= 30.0f;					//	テストなので　終了位置をプレイヤーの位置より10.0f奥の位置にする
	
	//float rot = g_Player.rot.y;
	//if (rot == XM_PI) {
	//	endPos.z += DISTANCE_OF_RAYCAST_PLAYER;
	//}
	//else if (rot == 0.0f) {
	//	endPos.z -= DISTANCE_OF_RAYCAST_PLAYER;
	//}
	//else if (rot == XM_PI / 2) {
	//	endPos.x -= DISTANCE_OF_RAYCAST_PLAYER;
	//}
	//else {
	//	endPos.x += DISTANCE_OF_RAYCAST_PLAYER;
	//}

	endPos.x += sinf(g_Player.font.y) * DISTANCE_OF_RAYCAST_PLAYER;
	endPos.z -= cosf(g_Player.font.y) * DISTANCE_OF_RAYCAST_PLAYER;

	for (int i = 0; i < MESHWALL_MAX; i++) {
		if (meshwall[i].use) {
			isHit = RayCast(meshwall[i].vPos[0], meshwall[i].vPos[1], meshwall[i].vPos[2], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return TRUE;
			}
			isHit = RayCast(meshwall[i].vPos[1], meshwall[i].vPos[2], meshwall[i].vPos[3], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return TRUE;
			}
		}
	}


	return FALSE;
}


void SenkeihokanPlayer(PLAYER* player, int type) {
	// 使われているなら処理する
	if ((player->use == true) && (player->tblMax > 0))
	{	// 線形補間の処理
		int nowNo = (int)player->time;			// 整数分であるテーブル番号を取り出している
		int maxNo = player->tblMax;				// 登録テーブル数を数えている
		int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
		INTERPOLATION_DATA* tbl = g_MoveTblAdr[player->tblNo];	// 行動テーブルのアドレスを取得

		XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
		XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
		XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

		XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
		XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
		XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

		float nowTime = player->time - nowNo;	// 時間部分である少数を取り出している

		Pos *= nowTime;								// 現在の移動量を計算している
		Rot *= nowTime;								// 現在の回転量を計算している
		Scl *= nowTime;								// 現在の拡大率を計算している

		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		XMStoreFloat3(&player->pos, nowPos + Pos);

		// 計算して求めた回転量を現在の移動テーブルに足している
		XMStoreFloat3(&player->rot, nowRot + Rot);

		// 計算して求めた拡大率を現在の移動テーブルに足している
		XMStoreFloat3(&player->scl, nowScl + Scl);

		// frameを使て時間経過処理をする
		if (type == MOVE_BY_KEYBOARD) {
			if (g_isMove) {
				player->time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)player->time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					player->time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}
			}
			else {
				player->time = 0;
			}
		}
		else if (type == FULLOUT) {
			player->time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)player->time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				player->time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}
		}
	}
}

void DrawPlayerSingle(PLAYER* player) {
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(player->scl.x, player->scl.y, player->scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(player->rot.x, player->rot.y, player->rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(player->pos.x, player->pos.y, player->pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	if (player->parent != NULL)	// 子供だったら親と結合する
	{
		mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&player->parent->mtxWorld));
		// ↑
		// g_Player.mtxWorldを指している
	}

	XMStoreFloat4x4(&player->mtxWorld, mtxWorld);

	// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
	if (player->use == true) {
		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);
		// モデル描画
		DrawModel(&player->model);// ワールドマトリックスの初期化
	}

}

//　タイトル用　プレイヤーの頭をエネミーに向かう
void TurnHeadTo(PLAYER *head) {

}