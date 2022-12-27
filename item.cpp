//=============================================================================
//
// �G�l�~�[���f������ [item.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "item.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "meshwall.h"
#include "wall.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ITEM			"data/MODEL/item/ammo.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ITEM_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ITEM_OFFSET_Y		(17.0f)						// �G�l�~�[�̑��������킹��



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ITEM			g_Item[MAX_ITEM];				// �G�l�~�[

static BOOL			g_Load = FALSE;









//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
	// �{��
	for (int i = 0; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM, &g_Item[i].model);
		g_Item[i].load = true;
		g_Item[i].pos = XMFLOAT3(-50.0f + i * 50.0f, ITEM_OFFSET_Y, -40.0f);
		g_Item[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Item[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_Item[i].size = ITEM_SIZE;	// �����蔻��̑傫��
		g_Item[i].use = true;
		g_Item[i].hitBoxIndex = SetMeshBox(g_Item[i].pos, g_Item[i].rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), 28.0f * g_Item[i].scl.x, 8.0f * g_Item[i].scl.y, 18.0f * g_Item[i].scl.z, i);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);

		XMFLOAT3 pos = g_Item[i].pos;
		pos.y -= (ITEM_OFFSET_Y - 0.1f);
		
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].load)
		{
			UnloadModel(&g_Item[i].model);
			g_Item[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void)
{
	
}



//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Item[i].scl.x, g_Item[i].scl.y, g_Item[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Item[i].rot.x, g_Item[i].rot.y + XM_PI, g_Item[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Item[i].pos.x, g_Item[i].pos.y, g_Item[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Item[i].mtxWorld, mtxWorld);

		DrawModel(&g_Item[i].model);
		
	}



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ITEM *GetItem()
{
	return &g_Item[0];
}

//�@�w�肳�ꂽ�A�C�e���𖳌�������
void DestoryItem(int num) {
	g_Item[num].use = false;
}


