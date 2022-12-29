//=============================================================================
//
// タイトル画面処理 [gamemodeUI.h]
// Author : 
//
//=============================================================================
#pragma once

enum UITexture
{
	UI_CROSS,	//	照準
	UI_GET,		//	入手	
	UI_HANDGUN,	//	ピストル
	UI_WEAPON_NONE,
	UI_MAX,
};


typedef struct _GMUI {
	bool			use;
	XMFLOAT3		pos;
	float			width;
	float			height;
	XMFLOAT4        diff;
}GMUI;

//*****************************************************************************
// マクロ定義
//*****************************************************************************




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGMUI(void);
void UninitGMUI(void);
void UpdateGMUI(void);
void DrawGMUI(void);
void TurnOnOffUI(int num, bool onoff);
void ChangeUIDiff(int num, XMFLOAT4 diff);

