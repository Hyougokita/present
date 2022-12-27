//=============================================================================
//
// WALLモデル処理 [WALL.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_WALL		(1)					// WALLの数

#define	WALL_SIZE		(5.0f)				// 当たり判定の大きさ

#define MESHWALL_MAX		(100)

#define MESHBOX_MAX			(10)	// 当たり判定用の立方体の数
//*****************************************************************************
// 構造体定義
//*****************************************************************************

//　長方形の構造体
typedef struct _HITBOX {
	XMFLOAT3 vPos[8];
}HITBOX;



typedef struct _MESHWALL {
	XMFLOAT3	vPos[4];
	BOOL		use;
}MESHWALL;

typedef struct _MESHBOX {
	XMFLOAT3		vPos[8];		//	頂点位置
	BOOL			use;			//	使用されているかどうか	
	char			itemNum;		//	番号
	char			itemType;		//	タイプ
}MESHBOX;


struct WALL
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	HITBOX				hitbox;

	float				size;				//　壁の大きさ

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitWall(void);
void UninitWall(void);
void UpdateWall(void);
void DrawWall(void);

WALL* GetWall(void);
MESHWALL* GetMeshWall(void);
MESHBOX* GetMeshBox(void);

void SetMeshWall(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 diff, float width, float height);
int SetMeshBox(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 diff, float width, float height, float depth, char itemNum, char itemType);
void DestoryMeshBox(int num);