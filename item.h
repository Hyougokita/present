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
	ITEM_TYPE_WINDOW,
	ITEM_TYPE_TEST_AMMO,
	ITEM_TYPE_WALL,
	ITEM_TYPE_DOOR,
	ITEM_TYPE_TABLE,
	ITEM_TYPE_CASTLE_WALL,
	ITEM_TYPE_CONTROLLER,
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

// ITEM WINDOW
enum ITEM_WINDOW_POSITION_TYPE {
	ITEM_WINDOW_TEST,
	ITEM_WINDOW_MAX
};

// ITEM TEST AMMO
enum ITEM_TESTAMMO_POSITION_TYPE
{
	ITEM_TEST_AMMO,
	ITEM_TEST_AMMO_MAX,
};

// ITEM HOUSE DOOR
enum ITEM_HOUSE_DOOR_TYPE
{
	ITEM_HOUSE_DOOR,
	ITEM_HOUSE_DOOR_OPENED,
	ITEM_HOUSE_DOOR_MAX
};

// ITEM CONTROLLER 
enum ITEM_CONTROLLER_TYPE {
	ITEM_CONTROLLER_OFF,
	ITEM_CONTROLLER_ON,
	ITEM_CONTROLLER_MAX
};

// ITEM TABLE
enum ITEM_TABLE_TYPE
{
	ITEM_TABLE,
	ITEM_TABLE_MAX
};

// ITEM DECORATION
enum ITEM_DECORATION_TYPE {
	ITEM_DECORATION_DOORTEST,
	ITEM_DECORATION_HOUSETEST,
	ITEM_DECORATION_ROOF,
	ITEM_DOOR_WAY,

	ITEM_DECORATION_TYPE_MAX,
};

enum ITEM_HOUSE_WALL_TYPE {
	ITEM_DOOR_RIGHT,
	ITEM_WINDOW_DOWN,
	ITEM_WINDOW_LEFT,
	ITEM_WINDOW_RIGHT,
	ITEM_WINDOW_UP,
	ITEM_DOOR_LEFT,
	ITEM_WALL_BACK,
	ITEM_WALL_LEFT,
	ITEM_DOOR_UP,
	ITEM_WALL_MAX
};

enum AUTO_MODEL_NAME
{
	AUTO_AMMO_TEST,
	AUTO_WALL_WINDOW,
	AUTO_WALL_DOOR_RIGHT,
	AUTO_WALL_WINDOW_DOWN,
	AUTO_WALL_WINDOW_LEFT,
	AUTO_WALL_WINDOW_RIGHT,
	AUTO_WALL_WINDOW_UP,
	AUTO_MAX
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
void ChangeItemUse(ITEM* item, bool use);
void DestoryItem(int num, char itemType);
void OpenCloseDoor();
void OpenCloseController();
bool BoxCheckWall(void);
bool CheckPlayerInHouse(void);
XMFLOAT3 GetHousePos(void);

bool IsDoorOpened(void);
bool IsControllerOpened(void);
void ChangeMatoToOn();