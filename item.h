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
#define MAX_ITEMBULLET		(5)					// エネミーの数
#define MAX_ENMEY_PARTS	(5)
#define	ITEM_SIZE		(5.0f)				// 当たり判定の大きさ

enum ITEM_TYPE {
	ITEM_AMMO_BOX,
	ITEM_TYPE_MAX
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
void DestoryItem(int num, char itemType);
