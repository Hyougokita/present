//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET		(256)	// �e�ő吔
#define DEBUG
//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			shadowIdx;		// �eID
	float       angleY;
	bool		use;			// �g�p���Ă��邩�ǂ�
#ifdef DEBUG
	int			count;
#endif // DEBUG



} BULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);
float AngleY();

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BULLET *GetBullet(void);

int GetCurMagazine();
int GetSubMagazine();
void AddSubMagazine(int num, int type);
void Reload(int type);