//=============================================================================
//
// タイトル画面処理 [gmui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "gamemodeUI.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(UI_MAX)				// テクスチャの数

//　照準
#define UI_CROSS_WIDTH				(32.0f)
#define UI_CROSS_HEIGHT				(32.0f)
#define UI_CROSS_POSITION			(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y,0.0f))

//  入手
#define UI_GET_SCALE				(0.3f)
#define UI_GET_WIDTH				(435.0f*UI_GET_SCALE)
#define UI_GET_HEIGHT				(207.0f*UI_GET_SCALE)
#define UI_GET_POSITION				(XMFLOAT3(SCREEN_CENTER_X + 1.0f * UI_GET_WIDTH,SCREEN_CENTER_Y,0.0f))

//  ピストル
#define UI_HANDGUN_SCALE			(0.3f)
#define UI_HANDGUN_WIDTH			(258.0f*UI_GET_SCALE)
#define UI_HANDGUN_HEIGHT			(188.0f*UI_GET_SCALE)
#define UI_HANDGUN_POSITION			(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_HANDGUN_WIDTH,SCREEN_HEIGHT - 50.0f,0.0f))

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
		"data/TEXTURE/gamemodeUI/cross.png",
		"data/TEXTURE/gamemodeUI/get.png",
		"data/TEXTURE/gamemodeUI/handgun.png",
};


static GMUI g_GMUI[UI_MAX];

static float uiTextureWidthList[UI_MAX] = {
	UI_CROSS_WIDTH,
	UI_GET_WIDTH,
	UI_HANDGUN_WIDTH,
};

static float uiTextureHeightList[UI_MAX] = {
	UI_CROSS_HEIGHT,
	UI_GET_HEIGHT,
	UI_HANDGUN_HEIGHT,
};

static XMFLOAT3 uiTexturePositionList[UI_MAX] = {
	UI_CROSS_POSITION,
	UI_GET_POSITION,
	UI_HANDGUN_POSITION,
};


bool g_Load = false;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGMUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	for (int i = 0; i < UI_MAX; i++) {
		g_GMUI[i].use = true;

		g_GMUI[i].pos = uiTexturePositionList[i];
		g_GMUI[i].width = uiTextureWidthList[i];
		g_GMUI[i].height = uiTextureHeightList[i];
		g_GMUI[i].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	
	g_GMUI[UI_CROSS].diff = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	g_GMUI[UI_HANDGUN].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGMUI(void)
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
// 更新処理
//=============================================================================
void UpdateGMUI(void)
{

	

#ifdef _DEBUG	// デバッグ情報を表示する

	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGMUI(void)
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

	for (int i = 0; i < UI_MAX; i++) {

		// 照準の描画
		{
			if (g_GMUI[i].use) {
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, g_GMUI[i].pos.x, g_GMUI[i].pos.y, g_GMUI[i].width, g_GMUI[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_GMUI[i].diff);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}


	


}




//　指定されたUIの表示のON/OFF
void TurnOnOffUI(int num,bool onoff) {
	g_GMUI[num].use = onoff;
}