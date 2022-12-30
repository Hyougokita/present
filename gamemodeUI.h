//=============================================================================
//
// タイトル画面処理 [gamemodeUI.h]
// Author : 
//
//=============================================================================
#pragma once

enum UITexture
{
	UI_CROSS,			//	照準
	UI_GET,				//	入手	
	UI_MOVE,			//  箱を移動させる
	UI_HANDGUN,			//	ピストル
	UI_WEAPON_NONE,		//	武器なしのアイコン
	UI_RELOAD_BG,		//　リロードのBG
	UI_RELOAD_FILL,		//　リロードの真ん中の部分
	UI_RELOAD_TEXT,		//  リロードのテキスト
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
