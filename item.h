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
#define MAX_ITEMBULLET		(5)					// �G�l�~�[�̐�
#define MAX_ENMEY_PARTS	(5)
#define	ITEM_SIZE		(5.0f)				// �����蔻��̑傫��

enum ITEM_TYPE {
	ITEM_AMMO_BOX,
	ITEM_TYPE_MAX
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
void DestoryItem(int num, char itemType);
