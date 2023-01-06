//=============================================================================
//
// スコア処理 [score.cpp]
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// キャラサイズ
#define TEXTURE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/numTexture001.png",
	"data/TEXTURE/slash.png",
};

static SCORE g_Score[SCORE_TYPE_MAX];


static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
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


	// 初期化
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
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateScore(void)
{
	//	現在の弾の数の更新
	g_Score[SCORE_CUR_MAGAZINE].score = GetCurMagazine();

	//	後備の弾の数の更新
	g_Score[SCORE_SUB_MAGAZINE].score = GetSubMagazine();


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	if (GetCanDrawUI()) {
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

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
		for (int j = 0; j < SCORE_TYPE_MAX; j++) {
			// 桁数分処理する
			int number = g_Score[j].score;
			for (int i = 0; i < g_Score[j].digit; i++)
			{
				// 今回表示する桁の数字
				float x = (float)(number % 10);

				// スコアの位置やテクスチャー座標を反映
				float px = g_Score[j].pos.x - g_Score[j].width * i;	// スコアの表示位置X
				float py = g_Score[j].pos.y;						// スコアの表示位置Y
				float pw = g_Score[j].width;						// スコアの表示幅
				float ph = g_Score[j].height;						// スコアの表示高さ

				float tw = 1.0f / 10;		// テクスチャの幅
				float th = 1.0f / 1;		// テクスチャの高さ
				float tx = x * tw;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// 次の桁へ
				number /= 10;
			}
		}

		// スラッシュを描く
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Score[SCORE_CUR_MAGAZINE].pos.x + 1.5f * TEXTURE_WIDTH, g_Score[SCORE_CUR_MAGAZINE].pos.y, g_Score[SCORE_CUR_MAGAZINE].width, g_Score[SCORE_CUR_MAGAZINE].height, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}









