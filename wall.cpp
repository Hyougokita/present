//=============================================================================
//
// �G�l�~�[���f������ [wall.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "wall.h"
#include "shadow.h"
#include "meshfield.h"
#include "meshwall.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_WALL			"data/MODEL/wall.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��


#define wall_OFFSET_Y		(0.0f)						// �n�ʂɒ��܂Ȃ��悤��

static MESHWALL g_MeshWall[MESHWALL_MAX];
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static WALL			g_Wall[MAX_WALL];				// WALL

static BOOL			g_Load = FALSE;





//=============================================================================
// ����������
//=============================================================================
HRESULT InitWall(void)
{
	for (int i = 0; i < MAX_WALL; i++)
	{
		LoadModel(MODEL_WALL, &g_Wall[i].model);
		g_Wall[i].load = true;

		g_Wall[i].pos = XMFLOAT3(0.0f * 30.0f, wall_OFFSET_Y, 0.0f);
		g_Wall[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Wall[i].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);


		g_Wall[i].size = WALL_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Wall[0].model, &g_Wall[0].diffuse[0]);

		XMFLOAT3 pos = g_Wall[i].pos;
		pos.y -= (wall_OFFSET_Y - 0.1f);


		g_Wall[i].use = false;			// true:�����Ă�

	}

	for (int i = 0; i < MESHWALL_MAX; i++) {
		g_MeshWall[i].use = false;
	}


	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitWall(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_WALL; i++)
	{
		if (g_Wall[i].load)
		{
			UnloadModel(&g_Wall[i].model);
			g_Wall[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateWall(void)
{
	

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawWall(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_WALL; i++)
	{
		if (g_Wall[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Wall[i].scl.x, g_Wall[i].scl.y, g_Wall[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Wall[i].rot.x, g_Wall[i].rot.y + XM_PI, g_Wall[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Wall[i].pos.x, g_Wall[i].pos.y, g_Wall[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Wall[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Wall[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
WALL* GetWall()
{
	return &g_Wall[0];
}

void SetMeshWall(XMFLOAT3 pos,XMFLOAT3 rot,XMFLOAT4 diff,float width,float height) {
	for (int i = 0; i < MESHWALL_MAX; i++) {
		if (g_MeshWall[i].use == false) {
			InitMeshWall(pos, rot, diff, 1, 1, width, height);
			// ���_���W�̕ۑ�
			for (int nCntVtxY = 0; nCntVtxY < 2; nCntVtxY++)
			{
				for (int nCntVtxX = 0; nCntVtxX < 2; nCntVtxX++)
				{
					// ���_���W�̐ݒ�
					g_MeshWall[i].vPos[nCntVtxY*2+nCntVtxX].x = -0.5 * width + nCntVtxX * width;
					g_MeshWall[i].vPos[nCntVtxY*2+nCntVtxX].y = 1 * height - nCntVtxY * height;
					g_MeshWall[i].vPos[nCntVtxY*2+nCntVtxX].z = 0.0f;
				}
			}
			g_MeshWall[i].use = true;
		}
		return;
	}
}

MESHWALL* GetMeshWall() {
	return &g_MeshWall[0];
}