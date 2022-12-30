//=============================================================================
//
// �^�C�g����ʏ��� [gamemodeUI.h]
// Author : 
//
//=============================================================================
#pragma once

enum UITexture
{
	UI_CROSS,			//	�Ə�
	UI_GET,				//	����	
	UI_MOVE,			//  �����ړ�������
	UI_HANDGUN,			//	�s�X�g��
	UI_WEAPON_NONE,		//	����Ȃ��̃A�C�R��
	UI_RELOAD_BG,		//�@�����[�h��BG
	UI_RELOAD_FILL,		//�@�����[�h�̐^�񒆂̕���
	UI_RELOAD_TEXT,		//  �����[�h�̃e�L�X�g
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
void ChangeUIWidth(int num, float uw);
XMFLOAT4 GetUIDiff(int num);
void TurnReloadUIOnOff(bool status);
