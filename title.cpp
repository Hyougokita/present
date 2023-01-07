//=============================================================================
//
// タイトル画面処理 [title.cpp]
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 

#define TEXTURE_SELECITON_SACLE				(1.0f)
#define TEXTURE_WIDTH_SELECTION				(411.0f * TEXTURE_SELECITON_SACLE)			// ロゴサイズ
#define TEXTURE_HEIGHT_SELECTION			(79.0f * TEXTURE_SELECITON_SACLE)			// 

#define TEXTURE_TITLE_SACLE					(1.0f)
#define TEXTURE_WIDTH_TITLE					(647.0f * TEXTURE_TITLE_SACLE)
#define TEXTURE_HEIGHT_TITLE				(146.0f * TEXTURE_TITLE_SACLE)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************


// 動かない部分
enum TITLE_TEXTURE_TITLE
{
	TITLE_BG,
	TITLE_TITLE,
	TITLE_TEXTURE_MAX,
};

//　選択部分
enum TITLE_TEXTURE_SELECTION
{
	SELECTION_START,
	SELECTION_TUTORIAL,
	SELECTION_OPTION,
	SELECTION_EXIT,
	SELECTION_TEXTURE_MAX
};

static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[SELECTION_TEXTURE_MAX + TITLE_TEXTURE_MAX] = { NULL };	// テクスチャ情報



static char *g_TexturName[TITLE_TEXTURE_MAX + SELECTION_TEXTURE_MAX] = {
		"data/TEXTURE/gameTitle/titlebg.png",
		"data/TEXTURE/gameTitle/gametitle.png",
		"data/TEXTURE/gameTitle/gamestart.png",
		"data/TEXTURE/gameTitle/tutorial.png",
		"data/TEXTURE/gameTitle/option.png",
		"data/TEXTURE/gameTitle/exit.png",
};

// TITLE の構造体
typedef struct _TITLE {
	XMFLOAT3		pos;		// 位置
	float			width;		// 幅
	float			height;		// 高さ
	XMFLOAT4		diff;		// RGBA
	float			scl;		// スケール
	int				TexNo;		// テクスチャ番号
	bool			isSelected;	// 選択されている
	int				count;
}TITLE;

// 選択部分
static TITLE g_TitleSelection[SELECTION_TEXTURE_MAX];

// 動かない部分
static TITLE g_TitleStaticPic[TITLE_TEXTURE_MAX];


static BOOL						g_Load = FALSE;

int g_IsSelectNo = 0;
int g_PreSelectNo = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 動かない部分
	for (int i = 0; i < TITLE_TEXTURE_MAX; i++) {
		g_TitleStaticPic[i].pos = XMFLOAT3(SCREEN_WIDTH / 2, 100.0f, 0.0f);
		g_TitleStaticPic[i].scl = 1.0f;
		g_TitleStaticPic[i].width = TEXTURE_WIDTH_TITLE * g_TitleStaticPic[i].scl;
		g_TitleStaticPic[i].height = TEXTURE_HEIGHT_TITLE * g_TitleStaticPic[i].scl;
		g_TitleStaticPic[i].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_TitleStaticPic[i].TexNo = i;
	}

	// 選択部分
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




	// BGM再生
	PlaySound(SOUND_LABEL_BGM_sample000);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
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
	// 選択されているものが大きくなる
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
	{// Enter押したら、ステージを切り替える
		if (g_IsSelectNo == SELECTION_START) {
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
	// ゲームパッドで入力処理
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}








#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
	PrintDebugProc("Select No:%d", g_IsSelectNo);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleStaticPic[TITLE_BG].TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// タイトルのタイトルの描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleStaticPic[TITLE_TITLE].TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_TitleStaticPic[TITLE_TITLE].pos.x, g_TitleStaticPic[TITLE_TITLE].pos.y, 
			g_TitleStaticPic[TITLE_TITLE].width, g_TitleStaticPic[TITLE_TITLE].height, 0.0f, 0.0f, 1.0f, 1.0f, g_TitleStaticPic[TITLE_TITLE].diff);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//　選択部分の描画
	{
		for (int i = 0; i < SELECTION_TEXTURE_MAX; i++) {
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleSelection[i].TexNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, g_TitleSelection[i].pos.x, g_TitleSelection[i].pos.y,
				g_TitleSelection[i].width, g_TitleSelection[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_TitleSelection[i].diff);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}



}





