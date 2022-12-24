//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(1)					// �v���C���[�̐�

#define	PLAYER_SIZE		(5.0f)				// �����蔻��̑傫��


#define PLAYER_HEAD_HEIGHT	(10.0f)


//�@�v���C���[�W�����v�֘A
#define	JUMP_POWER				(8.0f)					//	�W�����v�̑傫��
#define G						(5.0f)					//�@�d��
#define PLAYER_JUMP_COUNT_MAX	(30)					//	�W�����v���Ԃ̏��
enum PLAYER_JUMP_TYPE {
	JUMP_NORMAL,		//	���ʂ̃W�����v
	JUMP_OVER_THE_WALL,	//	�ǂ��z����̃W�����v
	JUMP_NONE,				//	�W�����v���Ă��Ȃ�
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	// �ʒu�֌W
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			prePos;				// �ړ�����O�̈ʒu���L�^����
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	//�@�ړ�
	float				spd;				// �ړ��X�s�[�h
	XMFLOAT3			font;				// �ړ��p�����Ă������
	float				spdValue;			// �ړ��̊�b�l

	//�@�W�����v
	int					jumpType;			// �W�����v�̎��
	int					jumpCnt;			// �W�����v�̃J�E���g

	//�@���f��
	BOOL				load;
	DX11_MODEL			model;				// ���f�����

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	BOOL				use;				//�@�g�p����Ă��邩�ǂ���

	//�@�����蔻��
	float				size;		// �����蔻��̃T�C�Y

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time;		// ���`��ԗp
	int					tblNo;		// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;		// ���̃e�[�u���̃f�[�^��

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// �����������Ă��鏊
};

enum PARTS_NAME {
	RIGHT_HAND,
	LEFT_HAND,
	HEAD,
	LEFT_LEG,
	RIGHT_LEG
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

