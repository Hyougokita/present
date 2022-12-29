//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

#define	PLAYER_SIZE		(5.0f)				// 当たり判定の大きさ


#define PLAYER_HEAD_HEIGHT	(10.0f)


//　プレイヤージャンプ関連
#define	JUMP_POWER				(8.0f)					//	ジャンプの大きさ
#define G						(5.0f)					//　重力
#define PLAYER_JUMP_COUNT_MAX	(25)					//	ジャンプ時間の上限



enum PLAYER_JUMP_TYPE {
	JUMP_NORMAL,		//	普通のジャンプ
	JUMP_OVER_THE_WALL,	//	壁を越えるのジャンプ
	JUMP_NONE,				//	ジャンプしていない
};

enum PLAYER_WEAPON_TYPE
{
	WEAPON_NONE,
	WEAPON_HANDGUN,
	WEAPON_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	// 位置関係
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			prePos;				// 移動する前の位置を記録する
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	//	ウェポンの状態
	int					preWeapon;			// 切り替え前のウェポンを記録する
	int					curWeapon;			// 現在手にあるウェポン
	bool				haveHandgun;		// ピストルを持っているかどうか

	//	リロード
	bool				isReloading;		//　リロードしているかどうか
	int					reloadCount;		//	リロード用カウント

	//　射撃	
	float			    shootY;				//　射撃用方向管理	

	//　移動
	float				spd;				// 移動スピード
	XMFLOAT3			front;				// 移動用向いている方向
	float				spdValue;			// 移動の基礎値

	//　ジャンプ
	int					jumpType;			// ジャンプの種類
	int					jumpCnt;			// ジャンプのカウント

	//　モデル
	BOOL				load;
	DX11_MODEL			model;				// モデル情報

	int					shadowIdx;			// 影のインデックス番号

	BOOL				use;				//　使用されているかどうか

	//　当たり判定
	float				size;		// 当たり判定のサイズ

	// 階層アニメーション用のメンバー変数
	float				time;		// 線形補間用
	INTERPOLATION_DATA* tblAdr;		// 行動データのテーブル番号
	int					tblMax;		// そのテーブルのデータ数

	float				rotX;		// 銃を持っている右手姿勢制御用X方向rot

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// 自分が立っている所
};

enum PARTS_NAME {
	RIGHT_HAND,
	LEFT_HAND,
	HEAD,
	LEFT_LEG,
	RIGHT_LEG,
	HAND_GUN
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

