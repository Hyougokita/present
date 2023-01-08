//=============================================================================
//
// �^�C�g����ʏ��� [gmui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "gamemodeUI.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(UI_MAX)				// �e�N�X�`���̐�

//�@�Ə�
#define UI_CROSS_WIDTH				(32.0f)
#define UI_CROSS_HEIGHT				(32.0f)
#define UI_CROSS_POSITION			(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y,0.0f))

//  ����
#define UI_GET_SCALE				(0.3f)
#define UI_GET_WIDTH				(435.0f*UI_GET_SCALE)
#define UI_GET_HEIGHT				(207.0f*UI_GET_SCALE)
#define UI_GET_POSITION				(XMFLOAT3(SCREEN_CENTER_X + 1.0f * UI_GET_WIDTH,SCREEN_CENTER_Y,0.0f))

//  �s�X�g��
#define UI_HANDGUN_SCALE			(0.3f)
#define UI_HANDGUN_WIDTH			(258.0f*UI_GET_SCALE)
#define UI_HANDGUN_HEIGHT			(188.0f*UI_GET_SCALE)
#define UI_HANDGUN_POSITION			(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_HANDGUN_WIDTH,SCREEN_HEIGHT - UI_HANDGUN_HEIGHT- 100.0f,0.0f))

//�@�E�F�|���Ȃ�
#define UI_WEAPON_NONE_POSITION		(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_HANDGUN_WIDTH,SCREEN_HEIGHT - 100.0f ,0.0f))

//  Reload
#define UI_RELOAD_SCALE				(0.5f)
#define UI_RELOAD_WIDTH				(333.0f * UI_RELOAD_SCALE)
#define UI_RELOAD_HEIGHT			(49.0f * UI_RELOAD_SCALE)
#define UI_RELOAD_BG_POSITION		(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y + UI_RELOAD_HEIGHT + 20.0f,0.0f))
#define UI_RELOAD_TEXT_POSITION		(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y + UI_RELOAD_HEIGHT * 2 + 20.0f,0.0f))

//	Lock
#define UI_LOCK_SCALE				(0.1f)
#define UI_LOCK_WIDTH				(5243.0f * UI_LOCK_SCALE)
#define UI_LOCK_HEIGHT				(334.0f * UI_LOCK_SCALE)
#define UI_LOCK_POSITION			(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y - UI_CROSS_HEIGHT,0.0f))

// �`���[�g���A���pUI
#define UI_TUTORIAL_SCALE		(0.3f)
#define UI_TUTORIAL_WIDTH		(1156.0f * UI_TUTORIAL_SCALE)
#define UI_TUTORIAL_HEIGHT		(216.0f * UI_TUTORIAL_SCALE)
#define UI_TUTORIAL_POS			(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_TUTORIAL_WIDTH,100.0f,0.0f))
#define UI_TUTORIAL_POS2		(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_TUTORIAL_WIDTH,100.0f + UI_TUTORIAL_HEIGHT,0.0f))
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[UI_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[UI_MAX] = {
		"data/TEXTURE/gamemodeUI/cross.png",					//	�Ə�
		"data/TEXTURE/gamemodeUI/get.png",						//	����	
		"data/TEXTURE/gamemodeUI/move.png",						//  �����ړ�������
		"data/TEXTURE/gamemodeUI/open.png",						//	�h�A�ȂǊJ��
		"data/TEXTURE/gamemodeUI/handgun.png",					//	�s�X�g��
		"data/TEXTURE/gamemodeUI/weaponNone.png",				//	����Ȃ��̃A�C�R��
		"data/TEXTURE/gamemodeUI/loading_background.png",		//�@�����[�h��BG
		"data/TEXTURE/gamemodeUI/loading_fill.png",				//�@�����[�h�̐^�񒆂̕���
		"data/TEXTURE/gamemodeUI/loading_text.png",				//  �����[�h�̃e�L�X�g
		"data/TEXTURE/gamemodeUI/lock.png",						//	�h�A�����b�N����Ă���e�L�X�g
		"data/TEXTURE/gamemodeUI/close.png",					//	�h�A�ȂǕ���

		//�`���[�g���A���p
		"data/TEXTURE/gamemodeUI/movetutorial.png",				//�@�ړ�
		"data/TEXTURE/gamemodeUI/jumptutorial.png",				//	�W�����v
		"data/TEXTURE/gamemodeUI/gettutorial.png",				//	�A�C�e���Ƃ̂����
		"data/TEXTURE/gamemodeUI/changeweapontutorial.png",		//�@����̐؂�ւ�
		"data/TEXTURE/gamemodeUI/reloadtutorial.png",			//�@�����[�h

};


static GMUI g_GMUI[UI_MAX];

static float uiTextureWidthList[UI_MAX] = {
	UI_CROSS_WIDTH,		//	�Ə�
	UI_GET_WIDTH,		//	����	
	UI_GET_WIDTH,		//  �����ړ�������
	UI_GET_WIDTH,		//	�h�A�ȂǊJ��
	UI_HANDGUN_WIDTH,	//	�s�X�g��
	UI_HANDGUN_WIDTH,	//	����Ȃ��̃A�C�R��
	UI_RELOAD_WIDTH,	//�@�����[�h��BG
	UI_RELOAD_WIDTH,	//�@�����[�h�̐^�񒆂̕���
	UI_RELOAD_WIDTH,	//  �����[�h�̃e�L�X�g
	UI_LOCK_WIDTH,		//	�h�A�����b�N����Ă���e�L�X�g
	UI_GET_WIDTH,		//	�h�A�ȂǕ���
	//�`���[�g���A���p
	UI_TUTORIAL_WIDTH,	//�@�ړ�
	UI_TUTORIAL_WIDTH,	//	�W�����v
	UI_TUTORIAL_WIDTH,	//	�A�C�e���Ƃ̂����
	UI_TUTORIAL_WIDTH,	//�@����̐؂�ւ�
	UI_TUTORIAL_WIDTH,	//�@�����[�h
};

static float uiTextureHeightList[UI_MAX] = {
	UI_CROSS_HEIGHT,   //	�Ə�
	UI_GET_HEIGHT,	   //	����	
	UI_GET_HEIGHT,	   //  �����ړ�������
	UI_GET_HEIGHT,	   //	�h�A�ȂǊJ��
	UI_HANDGUN_HEIGHT, //	�s�X�g��
	UI_HANDGUN_HEIGHT, //	����Ȃ��̃A�C�R��
	UI_RELOAD_HEIGHT,  //�@�����[�h��BG
	UI_RELOAD_HEIGHT,  //�@�����[�h�̐^�񒆂̕���
	UI_RELOAD_HEIGHT,  //  �����[�h�̃e�L�X�g
	UI_LOCK_HEIGHT,	   //	�h�A�����b�N����Ă���e�L�X�g
	UI_GET_HEIGHT,     //	�h�A�ȂǕ���
	//�`���[�g���A���p
	UI_TUTORIAL_HEIGHT,//�@�ړ�
	UI_TUTORIAL_HEIGHT,//	�W�����v
	UI_TUTORIAL_HEIGHT,//	�A�C�e���Ƃ̂����
	UI_TUTORIAL_HEIGHT,//�@����̐؂�ւ�
	UI_TUTORIAL_HEIGHT,//�@�����[�h
};

static XMFLOAT3 uiTexturePositionList[UI_MAX] = {
	UI_CROSS_POSITION,			//	�Ə�
	UI_GET_POSITION,			//	����	
	UI_GET_POSITION,			//  �����ړ�������
	UI_GET_POSITION,			//	�h�A�ȂǊJ��
	UI_HANDGUN_POSITION,		//	�s�X�g��
	UI_WEAPON_NONE_POSITION,	//	����Ȃ��̃A�C�R��
	UI_RELOAD_BG_POSITION,		//�@�����[�h��BG
	UI_RELOAD_BG_POSITION,		//�@�����[�h�̐^�񒆂̕���
	UI_RELOAD_TEXT_POSITION,	//  �����[�h�̃e�L�X�g
	UI_LOCK_POSITION,			//	�h�A�����b�N����Ă���e�L�X�g
	UI_GET_POSITION,			//	�h�A�ȂǕ���
	//�`���[�g���A���p
	UI_TUTORIAL_POS,			//�@�ړ�
	UI_TUTORIAL_POS2,			//	�W�����v
	UI_TUTORIAL_POS,			//	�A�C�e���Ƃ̂����
	UI_TUTORIAL_POS,			//�@����̐؂�ւ�
	UI_TUTORIAL_POS,			//�@�����[�h
};

static bool g_CanDrawUI = true;

bool g_Load = false;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGMUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	for (int i = 0; i < UI_MAX; i++) {
		g_GMUI[i].use = true;

		g_GMUI[i].pos = uiTexturePositionList[i];
		g_GMUI[i].width = uiTextureWidthList[i];
		g_GMUI[i].height = uiTextureHeightList[i];
		g_GMUI[i].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	}
	
	g_GMUI[UI_CROSS].diff = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	g_GMUI[UI_HANDGUN].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	g_GMUI[UI_HANDGUN].use = false;
	g_GMUI[UI_WEAPON_NONE].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.9f);

	g_GMUI[UI_RELOAD_BG].use = false;
	g_GMUI[UI_RELOAD_FILL].use = false;
	g_GMUI[UI_RELOAD_TEXT].use = false;

	g_GMUI[UI_DOOR_LOCK_TEXT].use = false;

	g_GMUI[UI_GET].use = false;
	g_GMUI[UI_OPEN].use = false;
	g_GMUI[UI_MOVE].use = false;
	g_GMUI[UI_CLOSE].use = false;

	g_GMUI[UI_GET_TUTORIAL].use = false;
	g_GMUI[UI_CHANGEWEAPON_TUTORIAL].use = false;
	g_GMUI[UI_RELOAD_TUTORIAL].use = false;




	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGMUI(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGMUI(void)
{
	// ���b�Z�[�W�����̎��Ԃɕ\�����ꂽ��@������
	if (g_GMUI[UI_DOOR_LOCK_TEXT].use) {
		g_GMUI[UI_DOOR_LOCK_TEXT].count++;
		if (g_GMUI[UI_DOOR_LOCK_TEXT].count >= 50) {
			g_GMUI[UI_DOOR_LOCK_TEXT].count = 0;
			g_GMUI[UI_DOOR_LOCK_TEXT].use = false;
		}
	}


	

#ifdef _DEBUG	// �f�o�b�O����\������

	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGMUI(void)
{
	if (g_CanDrawUI) {
		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// �}�g���N�X�ݒ�
		SetWorldViewProjection2D();

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		for (int i = 0; i < UI_MAX; i++) {

			// �Ə��̕`��
			{
				if (g_GMUI[i].use) {
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					// UI_FILL �̏ꍇ
					if (i == UI_RELOAD_FILL) {
						SetSpriteLeftTopColor(g_VertexBuffer,
							g_GMUI[i].pos.x - uiTextureWidthList[i] * 0.5f,
							g_GMUI[i].pos.y - uiTextureHeightList[i] * 0.5f,
							g_GMUI[i].width, g_GMUI[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_GMUI[i].diff);
					}
					//	���̑��̏ꍇ
					else {
						SetSpriteColor(g_VertexBuffer, g_GMUI[i].pos.x, g_GMUI[i].pos.y, g_GMUI[i].width, g_GMUI[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_GMUI[i].diff);
					}

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);
				}
			}
		}

	}
	


}




//�@�w�肳�ꂽUI�̕\����ON/OFF
void TurnOnOffUI(int num,bool onoff) {
	g_GMUI[num].use = onoff;
}

// UI�̃A���t�@�l��ύX����֐�
void ChangeUIDiff(int num, XMFLOAT4 diff) {
	g_GMUI[num].diff = diff;
}

// UI��width�l��ύX����֐�
void ChangeUIWidth(int num, float scl) {
	g_GMUI[num].width = uiTextureWidthList[num] * scl;
}

// �w�肳�ꂽUI��diff�l�𓾂�
XMFLOAT4 GetUIDiff(int num) {
	return g_GMUI[num].diff;
}

// �����[�h��UI��ON/OFF
void TurnReloadUIOnOff(bool status) {
	g_GMUI[UI_RELOAD_BG].use = status;
	g_GMUI[UI_RELOAD_FILL].use = status;
	g_GMUI[UI_RELOAD_TEXT].use = status;

	g_GMUI[UI_RELOAD_BG].diff.w = 1.0f;
	g_GMUI[UI_RELOAD_FILL].diff.w = 1.0f;
	g_GMUI[UI_RELOAD_TEXT].diff.w = 1.0f;

}

void TurnOnOffUIAll(bool OnOff) {
	g_CanDrawUI = OnOff;
}

bool GetCanDrawUI() {
	return g_CanDrawUI;
}

void CheckPlayerTutorialStage(void) {
	PLAYER* player = GetPlayer();
	if (player->moveStage == false && player->changeWeaponStage == false && player->reloadStage == false) {

	}
}