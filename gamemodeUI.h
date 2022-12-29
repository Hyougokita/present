//=============================================================================
//
// �^�C�g����ʏ��� [gamemodeUI.h]
// Author : 
//
//=============================================================================
#pragma once

enum UITexture
{
	UI_CROSS,	//	�Ə�
	UI_GET,		//	����	
	UI_HANDGUN,	//	�s�X�g��
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
// �}�N����`
//*****************************************************************************




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGMUI(void);
void UninitGMUI(void);
void UpdateGMUI(void);
void DrawGMUI(void);
void TurnOnOffUI(int num, bool onoff);
void ChangeUIDiff(int num, XMFLOAT4 diff);

