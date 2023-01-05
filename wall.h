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

#define MESHBOX_MAX			(80)	// �����蔻��p�̗����̂̐�
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

enum DATA_NAME
{
	DATA_AMMO_TEST,
	DATA_WALL_WINDOW,
	DATA_WALL_DOOR_RIGHT,
	DATA_WALL_WINDOW_DOWN,
	DATA_WALL_WINDOW_LEFT,
	DATA_WALL_WINDOW_RIGHT,
	DATA_WALL_WINDOW_UP,
	DATA_WALL_DOOR_LEFT,
	DATA_WALL_BACK,
	DATA_WALL_LEFT,
	DATA_WALL_DOOR_UP,
	DATA_DOOR,
	DATA_TABLE,
	DATA_DOOR_OPENED,
	DATA_CASTLE_WALL,
	DATA_CONTROLLER_OFF,
	DATA_CONTROLLER_ON,
	DATA_MAX
};


//�@�����`�̍\����
typedef struct _HITBOX {
	XMFLOAT3 vPos[8];
}HITBOX;



typedef struct _MESHWALL {
	XMFLOAT3	vPos[4];
	BOOL		use;
}MESHWALL;

typedef struct _MESHBOX {
	XMFLOAT3		pos;			//	���S�ʒu
	XMFLOAT3		vPos[8];		//	���_�ʒu
	BOOL			use;			//	�g�p����Ă��邩�ǂ���	
	char			itemNum;		//	�ԍ�
	char			itemType;		//	�^�C�v
}MESHBOX;

typedef struct _MESHTEST{

	XMFLOAT3		pos;
	XMFLOAT3		vPos[8];
}MESHTEST;

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
int SetMeshBoxFromData(XMFLOAT3 pos, XMFLOAT3 rot, float scl, int itemNum, int itemType, int dataNum);