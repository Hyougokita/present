//=============================================================================
//
// WALL���f������ [WALL.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_WALL		(1)					// WALL�̐�

#define	WALL_SIZE		(5.0f)				// �����蔻��̑傫��

#define MESHWALL_MAX		(100)

#define MESHBOX_MAX			(10)	// �����蔻��p�̗����̂̐�
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//�@�����`�̍\����
typedef struct _HITBOX {
	XMFLOAT3 vPos[8];
}HITBOX;



typedef struct _MESHWALL {
	XMFLOAT3	vPos[4];
	BOOL		use;
}MESHWALL;

typedef struct _MESHBOX {
	XMFLOAT3		vPos[8];		//	���_�ʒu
	BOOL			use;			//	�g�p����Ă��邩�ǂ���	
	char			itemNum;		//	�ԍ�
	char			itemType;		//	�^�C�v
}MESHBOX;


struct WALL
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	HITBOX				hitbox;

	float				size;				//�@�ǂ̑傫��

};

//*****************************************************************************
// �v���g�^�C�v�錾
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