//=============================================================================
//
// エネミーモデル処理 [item.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ITEMBULLET		(1)					// エネミーの数
#define MAX_ENMEY_PARTS	(5)
#define	ITEM_SIZE		(5.0f)				// 当たり判定の大きさ

// ITEM TYPE
enum ITEM_TYPE {
	ITEM_AMMO_BOX,
	ITEM_HAND_GUN,
	ITEM_TYPE_BOX,
	ITEM_TYPE_MAX
};

// ITEM AMMO
enum ITEM_AMMO_POSITION_TYPE
{
	ITEM_AMMO_TABLE,
	ITEM_AMMO_MAX,
};

// ITEM HANDGUN
enum ITEM_HANDGUN_POSITION_TYPE
{
	ITEM_HANDGUN_TABLE,
	ITEM_HANDGUN_MAX,
};

// ITEM BOX
enum ITEM_BOX_POSITION_TYPE {
	ITEM_BOX_TEST,
	ITEM_BOX_MAX,
};

// ITEM DECORATION
enum ITEM_DECORATION_TYPE {
	ITEM_DECORATION_TABLE00,
	ITEM_DECORATION_TYPE_MAX,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ITEM
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色


	float				size;				// 当たり判定の大きさ
	int					hitBoxIndex;		// HITBOXの何番目
	int					itemType;			// アイテムのタイプ


};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

ITEM *GetItemBullet(void);
ITEM* GetItemBox();
void DestoryItem(int num, char itemType);
