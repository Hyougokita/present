//=============================================================================
//
// �G�l�~�[���f������ [item.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ITEMBULLET		(1)					// �G�l�~�[�̐�
#define MAX_ENMEY_PARTS	(5)
#define	ITEM_SIZE		(5.0f)				// �����蔻��̑傫��

// ITEM TYPE
enum ITEM_TYPE {
	ITEM_AMMO_BOX,
	ITEM_HAND_GUN,
	ITEM_TYPE_BOX,
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

// ITEM DECORATION
enum ITEM_DECORATION_TYPE {
	ITEM_DECORATION_TABLE00,
	ITEM_DECORATION_TYPE_MAX,
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ITEM
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F


	float				size;				// �����蔻��̑傫��
	int					hitBoxIndex;		// HITBOX�̉��Ԗ�
	int					itemType;			// �A�C�e���̃^�C�v


};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

ITEM *GetItemBullet(void);
ITEM* GetItemBox();
void DestoryItem(int num, char itemType);
