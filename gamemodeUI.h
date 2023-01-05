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
	UI_OPEN,			//	�h�A�ȂǊJ��
	UI_HANDGUN,			//	�s�X�g��
	UI_WEAPON_NONE,		//	����Ȃ��̃A�C�R��
	UI_RELOAD_BG,		//�@�����[�h��BG
	UI_RELOAD_FILL,		//�@�����[�h�̐^�񒆂̕���
	UI_RELOAD_TEXT,		//  �����[�h�̃e�L�X�g
	UI_DOOR_LOCK_TEXT,	//	�h�A�����b�N����Ă���e�L�X�g
	UI_CLOSE,			//	�h�A�ȂǕ���
	UI_MAX,
};


typedef struct _GMUI {
	bool			use;		// �g�p����Ă��邩�ǂ���
	XMFLOAT3		pos;		//�@�\������Ă���ʒu
	float			width;		//	��
	float			height;		//  ����
	XMFLOAT4        diff;		//	RBGA
	float			uw;			//	
	int				count;		//	update�p�J�E���g
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
