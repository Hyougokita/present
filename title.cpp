//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "debugproc.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 

#define TEXTURE_SELECITON_SACLE				(1.0f)
#define TEXTURE_WIDTH_SELECTION				(411.0f * TEXTURE_SELECITON_SACLE)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_SELECTION			(79.0f * TEXTURE_SELECITON_SACLE)			// 

#define TEXTURE_TITLE_SACLE					(1.0f)
#define TEXTURE_WIDTH_TITLE					(647.0f * TEXTURE_TITLE_SACLE)
#define TEXTURE_HEIGHT_TITLE				(146.0f * TEXTURE_TITLE_SACLE)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


// �����Ȃ�����
enum TITLE_TEXTURE_TITLE
{
	TITLE_BG,
	TITLE_TITLE,
	TITLE_TEXTURE_MAX,
};

//�@�I�𕔕�
enum TITLE_TEXTURE_SELECTION
{
	SELECTION_START,
	SELECTION_TUTORIAL,
	SELECTION_OPTION,
	SELECTION_EXIT,
	SELECTION_TEXTURE_MAX
};

static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[SELECTION_TEXTURE_MAX + TITLE_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����



static char *g_TexturName[TITLE_TEXTURE_MAX + SELECTION_TEXTURE_MAX] = {
		"data/TEXTURE/gameTitle/titlebg.png",
		"data/TEXTURE/gameTitle/gametitle.png",
		"data/TEXTURE/gameTitle/gamestart.png",
		"data/TEXTURE/gameTitle/tutorial.png",
		"data/TEXTURE/gameTitle/option.png",
		"data/TEXTURE/gameTitle/exit.png",
};

// TITLE �̍\����
typedef struct _TITLE {
	XMFLOAT3		pos;		// �ʒu
	float			width;		// ��
	float			height;		// ����
	XMFLOAT4		diff;		// RGBA
	float			scl;		// �X�P�[��
	int				TexNo;		// �e�N�X�`���ԍ�
	bool			isSelected;	// �I������Ă���
	int				count;
}TITLE;

// �I�𕔕�
static TITLE g_TitleSelection[SELECTION_TEXTURE_MAX];

// �����Ȃ�����
static TITLE g_TitleStaticPic[TITLE_TEXTURE_MAX];


static BOOL						g_Load = FALSE;

int g_IsSelectNo = 0;
int g_PreSelectNo = 0;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < SELECTION_TEXTURE_MAX + TITLE_TEXTURE_MAX; i++)
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

	// �����Ȃ�����
	for (int i = 0; i < TITLE_TEXTURE_MAX; i++) {
		g_TitleStaticPic[i].pos = XMFLOAT3(SCREEN_WIDTH / 2, 100.0f, 0.0f);
		g_TitleStaticPic[i].scl = 1.0f;
		g_TitleStaticPic[i].width = TEXTURE_WIDTH_TITLE * g_TitleStaticPic[i].scl;
		g_TitleStaticPic[i].height = TEXTURE_HEIGHT_TITLE * g_TitleStaticPic[i].scl;
		g_TitleStaticPic[i].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_TitleStaticPic[i].TexNo = i;
	}

	// �I�𕔕�
	for (int i = 0; i < SELECTION_TEXTURE_MAX; i++) {
		g_TitleSelection[i].pos = XMFLOAT3(SCREEN_WIDTH / 2, 350.0f + i * TEXTURE_HEIGHT_SELECTION * 0.6f, 0.0f);
		g_TitleSelection[i].scl = 0.6f;
		g_TitleSelection[i].width = TEXTURE_WIDTH_SELECTION * g_TitleSelection[i].scl;
		g_TitleSelection[i].height = TEXTURE_HEIGHT_SELECTION * g_TitleSelection[i].scl;
		g_TitleSelection[i].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
		g_TitleSelection[i].TexNo = TITLE_TEXTURE_MAX + i;
		g_TitleSelection[i].isSelected = false;
	}

	g_TitleSelection[SELECTION_START].isSelected = true;
	g_TitleSelection[SELECTION_START].diff.w = 1.0f;




	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_sample000);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TITLE_TEXTURE_MAX; i++)
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
void UpdateTitle(void)
{



	if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_UP)) {
		g_PreSelectNo = g_IsSelectNo;
		g_IsSelectNo -= 1;
		if (g_IsSelectNo < 0) {
			g_IsSelectNo = SELECTION_TEXTURE_MAX - 1;
		}
	}

	if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_DOWN)) {
		g_PreSelectNo = g_IsSelectNo;
		g_IsSelectNo += 1;
		if (g_IsSelectNo >= SELECTION_TEXTURE_MAX) {
			g_IsSelectNo = 0;
		}
	}
	g_TitleSelection[g_PreSelectNo].isSelected = false;
	g_TitleSelection[g_IsSelectNo].isSelected = true;
	// �I������Ă�����̂��傫���Ȃ�
	for (int i = 0; i < SELECTION_TEXTURE_MAX; i++) {
		if (g_TitleSelection[i].isSelected) {
			g_TitleSelection[i].scl = 1.0f;
			g_TitleSelection[i].diff.w = 1.0f;
			g_TitleSelection[i].width = TEXTURE_WIDTH_SELECTION * g_TitleSelection[i].scl;
			g_TitleSelection[i].height = TEXTURE_HEIGHT_SELECTION * g_TitleSelection[i].scl;
		}
		else {
			g_TitleSelection[i].scl = 0.6f;
			g_TitleSelection[i].diff.w = 0.5f;
			g_TitleSelection[i].width = TEXTURE_WIDTH_SELECTION * g_TitleSelection[i].scl;
			g_TitleSelection[i].height = TEXTURE_HEIGHT_SELECTION * g_TitleSelection[i].scl;
		}
	}
	


	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		if (g_IsSelectNo == SELECTION_START) {
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
	// �Q�[���p�b�h�œ��͏���
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}








#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
	PrintDebugProc("Select No:%d", g_IsSelectNo);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleStaticPic[TITLE_BG].TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̃^�C�g���̕`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleStaticPic[TITLE_TITLE].TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_TitleStaticPic[TITLE_TITLE].pos.x, g_TitleStaticPic[TITLE_TITLE].pos.y, 
			g_TitleStaticPic[TITLE_TITLE].width, g_TitleStaticPic[TITLE_TITLE].height, 0.0f, 0.0f, 1.0f, 1.0f, g_TitleStaticPic[TITLE_TITLE].diff);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//�@�I�𕔕��̕`��
	{
		for (int i = 0; i < SELECTION_TEXTURE_MAX; i++) {
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleSelection[i].TexNo]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, g_TitleSelection[i].pos.x, g_TitleSelection[i].pos.y,
				g_TitleSelection[i].width, g_TitleSelection[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_TitleSelection[i].diff);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}



}





