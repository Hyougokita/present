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
	UI_RELOAD_BG,
	UI_RELOAD_FILL,
	UI_RELOAD_TEXT,
	UI_MAX,
};


typedef struct _GMUI {
	bool			use;
	XMFLOAT3		pos;
	float			width;
	float			height;
	XMFLOAT4        diff;
	float			uw;
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
void ChangeUIWidth(int num, float uw);
XMFLOAT4 GetUIDiff(int num);
void TurnReloadUIOnOff(bool status);
