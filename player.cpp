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
#include "gamemodeUI.h"
#include "item.h"
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
#define MODE_HAND_GUN			"data/MODEL/handgun2.obj"

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


#define PLAYER_PARTS_MAX	(6)								// プレイヤーのパーツの数
#define PLAYER_WEAPONS_MAX	(1)								// プレイヤーのパーツの数

#define DISTANCE_OF_RAYCAST_PLAYER  (20.0f)

XMFLOAT3 norScl = XMFLOAT3(0.4f, 0.4f, 0.4f);
XMFLOAT3 norScl2 = XMFLOAT3(1.0f, 1.0f, 1.0f);

//　初めてマウスロードする
bool g_FirstMouseLoad = false;

// アイテムを確認するための射線の長さ
#define DISTANCE_ITEM_CHECK_RAY	(100.0f)

// Reload time
#define PISTOL_RELOAD_TIME		(400)

static int reloadTimeList[WEAPON_MAX] = {
	0,
	PISTOL_RELOAD_TIME,
};

// ドア操作できるかどうかの判定
bool g_DoorIsLock = true;
bool g_PlayerInHouse = false;

// 窓の判定
bool g_HasWindow = false;

// ボックスに乗せているかどうか
bool g_PlayerOnBox = false;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL CheckTest(void);
BOOL CheckTestWall(void);
void SenkeihokanPlayer(PLAYER* player, int type);		//	線形補間計算
void DrawPlayerSingle(PLAYER* player);					//	プレーヤー単体の描画
int CheckItemHitBox(void);
void SetReloadBarFlash(int period);
bool CheckItemBoxHitBox(void);
void PlayerCheckMeshField(void);
void PlayerCheckBoxUp(void);
bool CheckWindow(void);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static PLAYER		g_Weapon[PLAYER_WEAPONS_MAX];	//	プレーヤーの武器用

static float		roty = 0.0f;

static LIGHT		g_Light;

bool g_isMove = false;

int g_checkItem = 0;
bool g_isCheckField = true;
int g_CheckItemBoxHitBox = -1;


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

	static INTERPOLATION_DATA right_hand_gun_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), norScl2, 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), norScl2, 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), norScl2, 15 },

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

	static INTERPOLATION_DATA* g_MoveWithGunTblAdr[] =
	{
		right_hand_gun_tbl_right,
		move_tbl_left,
		move_tbl_head,
		move_tbl_left_leg,
		move_tbl_right_leg,
		right_hand_gun_tbl_right

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
	g_Player.front = XMFLOAT3(0.0f,0.0f,0.0f);
	g_Player.use = TRUE;			// true:生きてる
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	// ウェポン用
	g_Player.curWeapon = 0;
	g_Player.preWeapon = 0;
	g_Player.haveHandgun = false;

	// 射撃用
	g_Player.shootY = XM_PI;

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

		g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, -50.0f };
		g_Player.rot = { 0.0f, 0.0f, 0.0f };
		g_Player.scl = XMFLOAT3(0.2f, 0.2f, 0.2f);

		g_Player.spd = 0.0f;			// 移動スピードクリア
		g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

		g_Player.use = true;

		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Player.pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		//g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
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
			g_Parts[i].tblAdr = g_MoveTblAdr[i];			// 再生する行動データテーブルNoをセット
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
#ifdef _DEBUG
#endif // _DEBUG


	g_Player.spdValue = VALUE_MOVE;
	// プレーヤーが家の中にいるかどうかを判断する
	g_PlayerInHouse = CheckPlayerInHouse();

	g_PlayerOnBox = false;

	// プレーヤーが向いている方向に窓があるかどうか
	g_HasWindow = false;
	MESHBOX* meshbox = GetMeshBox();

	//	アイテムとの当たり
	g_checkItem = CheckItemHitBox();
	if (g_checkItem != -1) {
		// アイテムを移動させる場合
		if (meshbox[g_checkItem].itemType == ITEM_TYPE_BOX)
			TurnOnOffUI(UI_MOVE, true);
		//　アイテムを得る場合
		else if (meshbox[g_checkItem].itemType == ITEM_AMMO_BOX || meshbox[g_checkItem].itemType == ITEM_HAND_GUN)
			TurnOnOffUI(UI_GET, true);
		else if(meshbox[g_checkItem].itemType == ITEM_TYPE_DOOR)
			TurnOnOffUI(UI_OPEN, true);
	}
	// UIの表示をOFFにする
	else {
		TurnOnOffUI(UI_GET, false);
		TurnOnOffUI(UI_MOVE, false);
		TurnOnOffUI(UI_OPEN, false);
	}

	
	//　Eキーでアイテムをとやり取りする
	if (GetKeyboardPress(DIK_E) && g_checkItem != -1) {
		// アイテムを移動させる場合
		if (meshbox[g_checkItem].itemType == ITEM_TYPE_BOX) {
			g_Player.spdValue = 0.5f;
			// 対応しているobjを探す
			ITEM* box = GetItemBox();
			XMFLOAT3 moveDistance = XMFLOAT3(g_Player.pos.x - g_Player.prePos.x, g_Player.pos.y - g_Player.prePos.y, g_Player.pos.z - g_Player.prePos.z);
			box[meshbox[g_checkItem].itemNum].pos.x += moveDistance.x;
			box[meshbox[g_checkItem].itemNum].pos.y += moveDistance.y;
			box[meshbox[g_checkItem].itemNum].pos.z += moveDistance.z;
			
			meshbox[g_checkItem].pos.x += moveDistance.x;
			meshbox[g_checkItem].pos.y += moveDistance.y;
			meshbox[g_checkItem].pos.z += moveDistance.z;

			for (int i = 0; i < 8; i++) {
				meshbox[g_checkItem].vPos[i].x += moveDistance.x;
				meshbox[g_checkItem].vPos[i].y += moveDistance.y;
				meshbox[g_checkItem].vPos[i].z += moveDistance.z;

			}
			if (BoxCheckWall()) {
				box[meshbox[g_checkItem].itemNum].pos.x -= moveDistance.x;
				box[meshbox[g_checkItem].itemNum].pos.y -= moveDistance.y;
				box[meshbox[g_checkItem].itemNum].pos.z -= moveDistance.z;

				meshbox[g_checkItem].pos.x -= moveDistance.x;
				meshbox[g_checkItem].pos.y -= moveDistance.y;
				meshbox[g_checkItem].pos.z -= moveDistance.z;

				for (int i = 0; i < 8; i++) {
					meshbox[g_checkItem].vPos[i].x -= moveDistance.x;
					meshbox[g_checkItem].vPos[i].y -= moveDistance.y;
					meshbox[g_checkItem].vPos[i].z -= moveDistance.z;

				}
			}
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[0].x, meshbox[g_checkItem].vPos[0].y, meshbox[g_checkItem].vPos[0].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[1].x, meshbox[g_checkItem].vPos[1].y, meshbox[g_checkItem].vPos[1].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[2].x, meshbox[g_checkItem].vPos[2].y, meshbox[g_checkItem].vPos[2].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[3].x, meshbox[g_checkItem].vPos[3].y, meshbox[g_checkItem].vPos[3].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[4].x, meshbox[g_checkItem].vPos[4].y, meshbox[g_checkItem].vPos[4].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[5].x, meshbox[g_checkItem].vPos[5].y, meshbox[g_checkItem].vPos[5].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[6].x, meshbox[g_checkItem].vPos[6].y, meshbox[g_checkItem].vPos[6].z);
			PrintDebugProc("meshbox x:%f,y:%f,z:%f\n", meshbox[g_checkItem].vPos[7].x, meshbox[g_checkItem].vPos[7].y, meshbox[g_checkItem].vPos[7].z);
		}
		//　ドアの操作
		else if(meshbox[g_checkItem].itemType == ITEM_TYPE_DOOR){

			if (g_PlayerInHouse || g_DoorIsLock == false) {
				g_DoorIsLock = false;
				OpenCloseDoor();
			}
			else {
				TurnOnOffUI(UI_DOOR_LOCK_TEXT, true);
			}

		}
		//　アイテムを得る場合
		else if(meshbox[g_checkItem].itemType == ITEM_AMMO_BOX || meshbox[g_checkItem].itemType == ITEM_HANDGUN_MAX){
			DestoryMeshBox(g_checkItem);
		}
	}

	



	CAMERA *cam = GetCamera();
	g_isMove = false;
	g_Player.spd = 0.0f;

#ifdef DEBUG
	if (CheckTest()) {
		isHitWall = true;
	}
	else {
		isHitWall = false;
	}

	if (CheckTestWall() || CheckItemBoxHitBox()) {
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

#endif

	
	{	// 押した方向にプレイヤーを移動させる
		// 押した方向にプレイヤーを向かせている所
		
		g_Player.front.y = roty + cam->rot.y;
		g_Player.shootY = cam->rot.y;

		g_Player.prePos = g_Player.pos;

		g_Player.pos.x += sinf(g_Player.front.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.front.y) * g_Player.spd;

		if (bCheckHitWall) {
			g_Player.pos = g_Player.prePos;
		}
	}

	// ウェポンの切り替え
	if ((GetKeyboardTrigger(DIK_1) || GetKeyboardTrigger(DIK_2))) {
		// 切り替え前のウェポンを記録する
		g_Player.preWeapon = g_Player.curWeapon;

		// 押したキーにより　ウェポンを切り替える　UIのアルファ値を変更する
		// ウェポンなし
		if (g_Player.haveHandgun && GetKeyboardTrigger(DIK_1)) {
			g_Player.curWeapon = WEAPON_NONE;
			ChangeUIDiff(UI_WEAPON_NONE, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.9f));
			ChangeUIDiff(UI_HANDGUN, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
		}
		//　ピストル
		if (g_Player.haveHandgun && GetKeyboardTrigger(DIK_2)) {
			g_Player.curWeapon = WEAPON_HANDGUN;
			ChangeUIDiff(UI_HANDGUN, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.9f));
			ChangeUIDiff(UI_WEAPON_NONE, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
		}

		// ウェポンを持っているかどうかにより　プレーヤーの姿勢制御
		// ウェポン持っていない場合
		if (g_Player.curWeapon != WEAPON_NONE) {
			for (int i = 0; i < PLAYER_PARTS_MAX; i++) {
				g_Parts[i].tblAdr = g_MoveWithGunTblAdr[i];
			}
		}
		//　ウェポンを持っている場合
		else {
			for (int i = 0; i < PLAYER_PARTS_MAX; i++) {
				g_Parts[i].tblAdr = g_MoveTblAdr[i];
			}
		}

		// 切り替え後の比較　リロード状態でウェポンを切り替えなら　リロードを中断する
		if (g_Player.isReloading && (g_Player.preWeapon != g_Player.curWeapon)) {
			g_Player.isReloading = false;
			TurnReloadUIOnOff(false);
		}
	}



	// 弾のリロード(リロードするとき射撃できない)
	if (GetKeyboardTrigger(DIK_R) && g_Player.isReloading == false) {
		//　リロード中ならリロードしない	かつ　ウェポンを持っている場合
		if (g_Player.isReloading == false && g_Player.curWeapon != WEAPON_NONE) {
			g_Player.reloadCount = 0;		// リロードカウントの初期化
			g_Player.isReloading = true;	// リロード状態になる
			// リロードのUIをONにする
			TurnReloadUIOnOff(true);
		}

	}

	//　リロード状態中なら
	if (g_Player.isReloading) {
		SetReloadBarFlash(20);
		// リロードの進捗によりUIのWidth値を調整する
		float scl = (float)g_Player.reloadCount / (float)reloadTimeList[g_Player.curWeapon];
		ChangeUIWidth(UI_RELOAD_FILL, scl);


		g_Player.reloadCount++;
		if (g_Player.reloadCount > reloadTimeList[g_Player.curWeapon]) {
			// リロード状態終了
			g_Player.isReloading = false;
			TurnReloadUIOnOff(false);
			// 現在と後備弾薬を計算する
			Reload(g_Player.curWeapon);
		}
	}



	// 弾発射処理
	if (IsMouseLeftTriggered())
	{
		// プレーヤーの向き
		XMFLOAT3 rot;
		rot = g_Player.front;
		rot.y = GetCamera()->rot.y + XM_PI;

		//　弾発射の始点
		XMFLOAT3 pos;
		pos = g_Player.pos;
		pos.y += PLAYER_HEAD_HEIGHT;

		// 弾を設置する
		SetBullet(pos, rot);
	}

	// 加速の処理(ダッシュ)
	if (GetKeyboardPress(DIK_LSHIFT)) {
		g_Player.spdValue = VALUE_MOVE * 2;
	}

	PlayerCheckBoxUp();
	// ジャンプ処理
	if(GetKeyboardTrigger(DIK_SPACE)){
		// ジャンプしていない場合
		if (g_Player.jumpType == JUMP_NONE) {
			//　普通のジャンプ
			g_Player.jumpType = JUMP_NORMAL;

			//	目の前に箱がある場合
			if (meshbox[g_checkItem].itemType == ITEM_TYPE_BOX) {
				g_Player.jumpType = JUMP_OVER_THE_BOX;
			}

			if (g_PlayerOnBox) {
				g_Player.jumpType = JUMP_OVER_THE_WALL;
				
			}
		}

	}
	g_isCheckField = true;
	// 普通のジャンプの場合
	if (g_Player.jumpType == JUMP_NORMAL) {
		g_Player.jumpCnt++;
		float jumpPower = (JUMP_POWER - g_Player.jumpCnt * 0.3f);
		if (jumpPower > G) {
			g_isCheckField = false;
		}
		g_Player.pos.y += jumpPower;
		if (g_Player.jumpCnt >= PLAYER_JUMP_COUNT_MAX) {
			g_Player.jumpCnt = 0;
			g_Player.jumpType = JUMP_NONE;
		}
	}

	//	箱に乗る場合
	if (g_Player.jumpType == JUMP_OVER_THE_BOX) {
		//g_Player.pos.x = (meshbox[g_checkItem].vPos[1].x - meshbox[g_checkItem].vPos[0].x) + meshbox[g_checkItem].vPos[0].x;
		g_Player.pos.y = (meshbox[g_checkItem].vPos[0].y + PLAYER_OFFSET_Y);
		//g_Player.pos.z = (meshbox[g_checkItem].vPos[4].z - meshbox[g_checkItem].vPos[0].z) + meshbox[g_checkItem].vPos[0].z;
		g_Player.pos.z = meshbox[g_checkItem].vPos[0].z + 10.0f;
		g_Player.pos.x = meshbox[g_checkItem].vPos[1].x + 10.0f;
		g_Player.jumpType = JUMP_NONE;
	}



	//　重力より落下する
	g_Player.pos.y -= G;

	if (g_Player.pos.y <= PLAYER_OFFSET_Y) {
		//g_Player.pos.y = PLAYER_OFFSET_Y;
	}


	if (g_Player.jumpType == JUMP_OVER_THE_WALL) {
		// 窓を越える場合
		if (g_PlayerOnBox && meshbox[g_checkItem].itemType == ITEM_TYPE_WINDOW) {
			g_Player.pos = GetHousePos();
			g_Player.jumpType = JUMP_NONE;
		}
	}

	if (g_isCheckField) {
		PlayerCheckMeshField();
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


	//g_Player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);

	angle = asinf(len);

	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&g_Player.Quaternion, quat);




#ifdef _DEBUG
	// デバッグ表示
	//PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y);
	PrintDebugProc("isHitWall: %d\n", isHitWall);
	PrintDebugProc("bCheckHitWall: %d\n", bCheckHitWall);
	PrintDebugProc("font.y:%f\n",g_Player.front.y);
	//PrintDebugProc("Mouse Pos: X:%f,Y:%f", g_MousePos.x, g_MousePos.y);
	PrintDebugProc("DisplaySize:%d\n", GetSystemMetrics(SM_CXSCREEN));
	PrintDebugProc("Player Jump Flag:%d\n", g_Player.jumpType);
	PrintDebugProc("Player Item Check:%d\n", g_checkItem);
	PrintDebugProc("Player Cur Weapon:%d\n", g_Player.curWeapon);
	PrintDebugProc("AngleY:%f\n", AngleY());
	PrintDebugProc("Player in house:%d\n", g_PlayerInHouse);
	PrintDebugProc("Player has window:%d\n", g_HasWindow);
	PrintDebugProc("g_CheckItemBoxHitBox:%d\n", g_CheckItemBoxHitBox);
	PrintDebugProc("PLAYER ON BOX:%d\n", g_PlayerOnBox);
	//PrintDebugProc("rot:X:%f,Y:%f,Z:%f,\n", rot.x, rot.y, rot.z);
	//PrintDebugProc("Player front:X:%f,Y:%f,Z:%f,\n", g_Player.front.x, g_Player.front.y, g_Player.front.z);
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
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, -GetCamera()->rot.y + XM_PI, g_Player.rot.z);
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
	
	// ピストルとピストルを持っている手を視点により回転させる
	g_Parts[RIGHT_HAND].rot.x += AngleY();
	g_Parts[HAND_GUN].rot.x += AngleY();
#ifdef _DEBUG
	PrintDebugProc("Part HandGun Rot X:%f\n", g_Parts[HAND_GUN].rot.x);
#endif // _DEBUG
	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		DrawPlayerSingle(&g_Parts[i]);

	}
	// 線形補間計算するため　元に戻す
	g_Parts[RIGHT_HAND].rot.x -= AngleY();
	g_Parts[HAND_GUN].rot.x -= AngleY();
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

//　レイキャスト（テスト用）
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

// プレーヤーと壁の判定（テスト用）
BOOL CheckTestWall() {
	MESHWALL* meshwall = GetMeshWall();
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）

	XMFLOAT3 startPos = g_Player.pos;	//　始点をプレイヤーの位置にする
	XMFLOAT3 endPos = g_Player.pos;


	endPos.x += sinf(g_Player.front.y) * DISTANCE_OF_RAYCAST_PLAYER;
	endPos.z -= cosf(g_Player.front.y) * DISTANCE_OF_RAYCAST_PLAYER;

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

// プレーヤーの階層アニメーション用線形補間
void SenkeihokanPlayer(PLAYER* player, int type) {
	// 使われているなら処理する
	if ((player->use == true) && (player->tblMax > 0))
	{	// 線形補間の処理
		int nowNo = (int)player->time;			// 整数分であるテーブル番号を取り出している
		int maxNo = player->tblMax;				// 登録テーブル数を数えている
		int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
		INTERPOLATION_DATA* tbl = player->tblAdr;	// 行動テーブルのアドレスを取得

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

//	プレーヤーの一つのパーツを描く関数
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

//  アイテム拾い用当たり判定ボックス
int CheckItemHitBox(void) {
	//プレーヤーが見ている方向へ射線を射出

	//　向いて方向
	XMFLOAT3 rot;
	rot = g_Player.front;
	rot.y = GetCamera()->rot.y + XM_PI;

	//	発射の始点
	XMFLOAT3 startPos;
	startPos = g_Player.pos;
	startPos.y += PLAYER_HEAD_HEIGHT;

	//  垂直の角度
	float angleY = AngleY();

	// 終了位置
	XMFLOAT3 endPos;
	endPos.x = startPos.x + sinf(rot.y) * DISTANCE_ITEM_CHECK_RAY;
	endPos.z = startPos.z - cosf(rot.y) * DISTANCE_ITEM_CHECK_RAY;
	endPos.y = startPos.y + sinf(angleY) * DISTANCE_ITEM_CHECK_RAY;

	// Meshbox取得
	MESHBOX* meshbox = GetMeshBox();

	//  Meshboxとの当たり判定
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	for (int i = 0; i < MESHBOX_MAX; i++) {
		if (meshbox[i].use) {
			//	計算量の減少(プレーヤーから一定の距離離れると計算しない)
			float vx, vz;
			vx = g_Player.pos.x - meshbox[i].pos.x;
			vz = g_Player.pos.z - meshbox[i].pos.z;
			float distance = pow(pow(vx, 2) + pow(vz, 2), 0.5f);
			if (distance > 1.5f * DISTANCE_ITEM_CHECK_RAY)
				continue;
			
			//裏
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[2], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[2], meshbox[i].vPos[3], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			//前
			isHit = RayCast(meshbox[i].vPos[4], meshbox[i].vPos[5], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			isHit = RayCast(meshbox[i].vPos[5], meshbox[i].vPos[6], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			//左
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[3], meshbox[i].vPos[4], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			isHit = RayCast(meshbox[i].vPos[3], meshbox[i].vPos[4], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			//右
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[2], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[5], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			//上
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[4], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[4], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			//底
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[3], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[6], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return i;
			}
		}
	}
	return -1;
}

//  壁などの当たり判定
bool CheckItemBoxHitBox(void) {
	//プレーヤーが見ている方向へ射線を射出

	//	発射の始点
	XMFLOAT3 startPos;
	startPos = g_Player.pos;
	startPos.y -= PLAYER_OFFSET_Y * 0.5f;

	// 終了位置
	XMFLOAT3 endPos = startPos;
	endPos.x = startPos.x + sinf(g_Player.front.y) * DISTANCE_OF_RAYCAST_PLAYER;
	endPos.z = startPos.z - cosf(g_Player.front.y) * DISTANCE_OF_RAYCAST_PLAYER;
	endPos.y -= PLAYER_OFFSET_Y * 0.5f;


	// Meshbox取得
	MESHBOX* meshbox = GetMeshBox();

	//  Meshboxとの当たり判定
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	for (int i = 0; i < MESHBOX_MAX; i++) {
		if (meshbox[i].use) {
			// 特定の物体しか判定しない
			if (meshbox[i].itemType != ITEM_TYPE_BOX && meshbox[i].itemType != ITEM_TYPE_WALL && meshbox[i].itemType != ITEM_TYPE_TABLE && meshbox[i].itemType != ITEM_TYPE_DOOR && meshbox[i].itemType != ITEM_TYPE_CASTLE_WALL)
				continue;
			//	計算量の減少(プレーヤーから一定の距離離れると計算しない)
			float vx, vz;
			vx = g_Player.pos.x - meshbox[i].pos.x;
			vz = g_Player.pos.z - meshbox[i].pos.z;
			float distance = pow(pow(vx, 2) + pow(vz, 2), 0.5f);
			if (distance > 1.5f * DISTANCE_ITEM_CHECK_RAY)
				continue;

			//裏
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[2], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[2], meshbox[i].vPos[3], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			//前
			isHit = RayCast(meshbox[i].vPos[4], meshbox[i].vPos[5], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[5], meshbox[i].vPos[6], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			//左
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[3], meshbox[i].vPos[4], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[3], meshbox[i].vPos[4], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			//右
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[2], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[5], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			//上
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[4], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[4], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			//底
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[3], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[6], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_CheckItemBoxHitBox = i;
				return true;
			}
		}
	}
	return false;
}

//	リロードのバーを点滅させる
void SetReloadBarFlash(int period) {
	int result = (g_Player.reloadCount % (period * 2)) / period;
	float result2 = ((float)(g_Player.reloadCount % period) / period);
	float diffW = 1.0f - (result2 * pow(-1,result));
	PrintDebugProc("result 1:%d\n", result);
	PrintDebugProc("result 2:%f\n", result2);
	PrintDebugProc("result 1:%f\n", diffW);
	ChangeUIDiff(UI_RELOAD_TEXT, XMFLOAT4(1.0f, 1.0f, 1.0f, diffW));
	ChangeUIDiff(UI_RELOAD_FILL, XMFLOAT4(1.0f, 1.0f, 1.0f, diffW));
}

//	プレーヤーと地面の判定
void PlayerCheckMeshField(void) {
	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	bool ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
#ifdef _DEBUG
	PrintDebugProc("ANS:%d\n", ans);
#endif // _DEBUG

	if (ans)
	{
		g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		//g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
	//	プレーヤーのクオータニオン用
	g_Player.UpVector = Normal;
}

// プレーヤーと箱の上面の判定
void PlayerCheckBoxUp(void) {
	MESHBOX* meshbox = GetMeshBox();
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	for (int i = 0; i < MESHBOX_MAX; i++) {
		if (meshbox[i].itemType != ITEM_TYPE_BOX) {
			continue;
		}
		//　boxとプレーヤー距離を計算する 一定の距離と離れると判定しない
		if (CheckDistance(g_Player.pos, meshbox[i].pos, 200.0f)) {
			//　射線はplayerの位置から足のところまで
			XMFLOAT3 startPos = g_Player.pos;
			XMFLOAT3 endPos = g_Player.pos;
			endPos.y -= PLAYER_OFFSET_Y;

			//	判定があればplayerを箱に乗せる
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_Player.pos.y = meshbox[i].vPos[0].y + PLAYER_OFFSET_Y;
				g_PlayerOnBox = true;
				return;
			}
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[4], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				g_Player.pos.y = meshbox[i].vPos[0].y + PLAYER_OFFSET_Y;
				g_PlayerOnBox = true;
				return;
			}
		}
	}
}

// 二つのobjの間の距離と指定した距離の比較　小さいならTrue
bool CheckDistance(XMFLOAT3 pos1, XMFLOAT3 pos2, float distance) {
	float vx, vz;
	vx = pos1.x -pos2.x;
	vz = pos1.z -pos2.z;
	float p1p2distance = pow(pow(vx, 2) + pow(vz, 2), 0.5f);
	if (p1p2distance > distance)
		return false;
	return true;
}

//　プレーヤーの位置から一定の距離の上に窓があるかどうかを確認する
bool CheckWindow(void) {
	//プレーヤーが見ている方向へ射線を射出

	//	発射の始点
	XMFLOAT3 startPos;
	startPos = g_Player.pos;
	startPos.y += PLAYER_HEAD_HEIGHT;

	// 終了位置
	XMFLOAT3 endPos = startPos;
	endPos.x = startPos.x + sinf(g_Player.front.y) * (DISTANCE_OF_RAYCAST_PLAYER * 3);
	endPos.z = startPos.z - cosf(g_Player.front.y) * (DISTANCE_OF_RAYCAST_PLAYER * 3);
	endPos.y += PLAYER_HEAD_HEIGHT;


	// Meshbox取得
	MESHBOX* meshbox = GetMeshBox();

	//  Meshboxとの当たり判定
	BOOL isHit;
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	for (int i = 0; i < MESHBOX_MAX; i++) {
		if (meshbox[i].use) {
			// 箱しか判定しない
			if (meshbox[i].itemType != ITEM_TYPE_WINDOW)
				continue;
			//	計算量の減少(プレーヤーから一定の距離離れると計算しない)
			float vx, vz;
			vx = g_Player.pos.x - meshbox[i].pos.x;
			vz = g_Player.pos.z - meshbox[i].pos.z;
			float distance = pow(pow(vx, 2) + pow(vz, 2), 0.5f);
			if (distance > 1.5f * DISTANCE_ITEM_CHECK_RAY)
				continue;

			//裏
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[2], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[2], meshbox[i].vPos[3], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			//前
			isHit = RayCast(meshbox[i].vPos[4], meshbox[i].vPos[5], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[5], meshbox[i].vPos[6], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			//左
			isHit = RayCast(meshbox[i].vPos[1], meshbox[i].vPos[3], meshbox[i].vPos[4], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[3], meshbox[i].vPos[4], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			//右
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[2], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[5], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			//上
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[1], meshbox[i].vPos[4], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[0], meshbox[i].vPos[4], meshbox[i].vPos[5], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			//底
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[3], meshbox[i].vPos[6], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
			isHit = RayCast(meshbox[i].vPos[2], meshbox[i].vPos[6], meshbox[i].vPos[7], startPos, endPos, &HitPosition, &Normal);
			if (isHit) {
				return true;
			}
		}
	}
	return false;
}

