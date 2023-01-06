//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "bullet.h"
#include "gamemodeUI.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/numTexture001.png",
	"data/TEXTURE/slash.png",
};

static SCORE g_Score[SCORE_TYPE_MAX];


static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
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


	// ������
	for (int i = 0; i < SCORE_TYPE_MAX; i++) {
		g_Score[i].use = true;
		g_Score[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Score[i].width = TEXTURE_WIDTH;
		g_Score[i].height = TEXTURE_HEIGHT;
		g_Score[i].score = 0;
		g_Score[i].digit = 2;
	}

	g_Score[SCORE_CUR_MAGAZINE].digit = 2;
	g_Score[SCORE_SUB_MAGAZINE].digit = 3;

	g_Score[SCORE_SUB_MAGAZINE].pos = XMFLOAT3(SCREEN_WIDTH - 1.0f * TEXTURE_WIDTH, SCREEN_HEIGHT - 0.5f * TEXTURE_HEIGHT, 0.0f);
	g_Score[SCORE_CUR_MAGAZINE].pos = XMFLOAT3(g_Score[SCORE_SUB_MAGAZINE].pos.x - (g_Score[SCORE_SUB_MAGAZINE].digit + 1.5f) * TEXTURE_WIDTH, g_Score[SCORE_SUB_MAGAZINE].pos.y, 0.0f);



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{
	//	���݂̒e�̐��̍X�V
	g_Score[SCORE_CUR_MAGAZINE].score = GetCurMagazine();

	//	����̒e�̐��̍X�V
	g_Score[SCORE_SUB_MAGAZINE].score = GetSubMagazine();


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
{
	if (GetCanDrawUI()) {
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

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
		for (int j = 0; j < SCORE_TYPE_MAX; j++) {
			// ��������������
			int number = g_Score[j].score;
			for (int i = 0; i < g_Score[j].digit; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Score[j].pos.x - g_Score[j].width * i;	// �X�R�A�̕\���ʒuX
				float py = g_Score[j].pos.y;						// �X�R�A�̕\���ʒuY
				float pw = g_Score[j].width;						// �X�R�A�̕\����
				float ph = g_Score[j].height;						// �X�R�A�̕\������

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				number /= 10;
			}
		}

		// �X���b�V����`��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Score[SCORE_CUR_MAGAZINE].pos.x + 1.5f * TEXTURE_WIDTH, g_Score[SCORE_CUR_MAGAZINE].pos.y, g_Score[SCORE_CUR_MAGAZINE].width, g_Score[SCORE_CUR_MAGAZINE].height, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}









