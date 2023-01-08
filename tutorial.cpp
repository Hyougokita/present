//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "wall.h"
#include "gamemodeUI.h"
#include "item.h"
#include "tutorial.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define DEBUG

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static bool	g_bPause = true;	// ポーズON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// 壁の初期化
	InitWall();

	// アイテムの初期化
	InitItem();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// UIの初期化
	InitGMUI();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_sample001);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial (void)
{
	// パーティクルの終了処理
	UninitParticle();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// 木の終了処理
	UninitTree();

	// UIの終了処理
	UninitGMUI();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	// 壁の終了
	UninitWall();

	// アイテムの終了処理
	UninitItem();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? false : true;
	}


#endif

	if (g_bPause == false)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// プレイヤーの更新処理
	UpdatePlayer();



	// 壁処理の更新
	UpdateMeshWall();



	// 弾の更新処理
	UpdateBullet();



	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();

	// 壁の更新
	UpdateWall();

	// アイテムの更新処理
	UpdateItem();

	// UIの更新処理
	UpdateGMUI();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();



	// プレイヤーの描画処理
	DrawPlayer();

	// アイテムの描画処理
	DrawItem();

	// 弾の描画処理
	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();



	// 壁の描画
	DrawWall();


	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(false);

	// ライティングを無効
	SetLightEnable(false);

	// スコアの描画処理
	DrawScore();

	// UIの描画処理
	DrawGMUI();


	// ライティングを有効に
	SetLightEnable(true);

	// Z比較あり
	SetDepthEnable(true);
}


void DrawTutorial(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y += GetCamera()->upDown + PLAYER_HEAD_HEIGHT;
	//	頭の高さ



	pos.x -= sinf(GetPlayer()->rot.y + GetCamera()->rot.y) * GetCamera()->len;
	pos.z += cosf(GetPlayer()->rot.y + GetCamera()->rot.y) * GetCamera()->len;
	SetCameraAT(pos);
	SetCamera();

	switch (g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawTutorial0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		//SetViewPort(TYPE_LEFT_HALF_SCREEN);
		//DrawGame0();

		// エネミー視点
		//pos = GetPlayer()->pos;
		pos = XMFLOAT3(171.0f, 27.0f, -356.0f);
		//pos.y = 0.0f;
		SetCameraAT2(pos);
		SetCamera();
		SetViewPort(TYPE_FULL_SCREEN);
		DrawTutorial0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawTutorial0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawTutorial0();
		break;

	}

}






