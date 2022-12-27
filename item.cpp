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
#include "bullet.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ITEM			"data/MODEL/item/ammo.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ITEM_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ITEM_OFFSET_Y		(17.0f)						// �G�l�~�[�̑��������킹��

#define	AMMO_BOX_ADD_NUM	(20)						// �e�{�b�N�X����������e�̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
void DrawItemSingle(ITEM* item);
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ITEM			g_ItemBullet[MAX_ITEMBULLET];				// �G�l�~�[

static BOOL			g_Load = FALSE;









//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
	// �{��
	for (int i = 0; i < MAX_ITEMBULLET; i++)
	{
		LoadModel(MODEL_ITEM, &g_ItemBullet[i].model);
		g_ItemBullet[i].load = true;
		g_ItemBullet[i].pos = XMFLOAT3(-50.0f + i * 50.0f, ITEM_OFFSET_Y, -40.0f);
		g_ItemBullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ItemBullet[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_ItemBullet[i].size = ITEM_SIZE;	// �����蔻��̑傫��
		g_ItemBullet[i].use = true;
		g_ItemBullet[i].hitBoxIndex = SetMeshBox(g_ItemBullet[i].pos, g_ItemBullet[i].rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), 28.0f * g_ItemBullet[i].scl.x, 8.0f * g_ItemBullet[i].scl.y, 18.0f * g_ItemBullet[i].scl.z, i, ITEM_AMMO_BOX);

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_ItemBullet[0].model, &g_ItemBullet[0].diffuse[0]);

		XMFLOAT3 pos = g_ItemBullet[i].pos;
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

	for (int i = 0; i < MAX_ITEMBULLET; i++)
	{
		if (g_ItemBullet[i].load)
		{
			UnloadModel(&g_ItemBullet[i].model);
			g_ItemBullet[i].load = false;
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


	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �e�{�b�N�X�̕`��
	for (int i = 0; i < MAX_ITEMBULLET; i++)
	{
		if (g_ItemBullet[i].use == false) continue;
		DrawItemSingle(&g_ItemBullet[i]);
	}



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ITEM *GetItemBullet()
{
	return &g_ItemBullet[0];
}

//�@�w�肳�ꂽ�A�C�e���𖳌�������
void DestoryItem(int num, char itemType) {
	if (itemType == ITEM_AMMO_BOX) {
		g_ItemBullet[num].use = false;
		AddSubMagazine(AMMO_BOX_ADD_NUM);
	}
}

void DrawItemSingle(ITEM* item) {
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(item->scl.x, item->scl.y, item->scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(item->rot.x, item->rot.y + XM_PI, item->rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(item->pos.x, item->pos.y, item->pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&item->mtxWorld, mtxWorld);

	DrawModel(&item->model);
}
