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
#include "player.h"

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
#define UI_HANDGUN_POSITION			(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_HANDGUN_WIDTH,SCREEN_HEIGHT - UI_HANDGUN_HEIGHT- 100.0f,0.0f))

//　ウェポンなし
#define UI_WEAPON_NONE_POSITION		(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_HANDGUN_WIDTH,SCREEN_HEIGHT - 100.0f ,0.0f))

//  Reload
#define UI_RELOAD_SCALE				(0.5f)
#define UI_RELOAD_WIDTH				(333.0f * UI_RELOAD_SCALE)
#define UI_RELOAD_HEIGHT			(49.0f * UI_RELOAD_SCALE)
#define UI_RELOAD_BG_POSITION		(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y + UI_RELOAD_HEIGHT + 20.0f,0.0f))
#define UI_RELOAD_TEXT_POSITION		(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y + UI_RELOAD_HEIGHT * 2 + 20.0f,0.0f))

//	Lock
#define UI_LOCK_SCALE				(0.1f)
#define UI_LOCK_WIDTH				(5243.0f * UI_LOCK_SCALE)
#define UI_LOCK_HEIGHT				(334.0f * UI_LOCK_SCALE)
#define UI_LOCK_POSITION			(XMFLOAT3(SCREEN_CENTER_X,SCREEN_CENTER_Y - UI_CROSS_HEIGHT,0.0f))

// チュートリアル用UI
#define UI_TUTORIAL_SCALE		(0.3f)
#define UI_TUTORIAL_WIDTH		(1156.0f * UI_TUTORIAL_SCALE)
#define UI_TUTORIAL_HEIGHT		(216.0f * UI_TUTORIAL_SCALE)
#define UI_TUTORIAL_POS			(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_TUTORIAL_WIDTH,100.0f,0.0f))
#define UI_TUTORIAL_POS2		(XMFLOAT3(SCREEN_WIDTH - 0.5f * UI_TUTORIAL_WIDTH,100.0f + UI_TUTORIAL_HEIGHT,0.0f))
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[UI_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[UI_MAX] = {
		"data/TEXTURE/gamemodeUI/cross.png",					//	照準
		"data/TEXTURE/gamemodeUI/get.png",						//	入手	
		"data/TEXTURE/gamemodeUI/move.png",						//  箱を移動させる
		"data/TEXTURE/gamemodeUI/open.png",						//	ドアなど開く
		"data/TEXTURE/gamemodeUI/handgun.png",					//	ピストル
		"data/TEXTURE/gamemodeUI/weaponNone.png",				//	武器なしのアイコン
		"data/TEXTURE/gamemodeUI/loading_background.png",		//　リロードのBG
		"data/TEXTURE/gamemodeUI/loading_fill.png",				//　リロードの真ん中の部分
		"data/TEXTURE/gamemodeUI/loading_text.png",				//  リロードのテキスト
		"data/TEXTURE/gamemodeUI/lock.png",						//	ドアがロックされているテキスト
		"data/TEXTURE/gamemodeUI/close.png",					//	ドアなど閉じる

		//チュートリアル用
		"data/TEXTURE/gamemodeUI/movetutorial.png",				//　移動
		"data/TEXTURE/gamemodeUI/jumptutorial.png",				//	ジャンプ
		"data/TEXTURE/gamemodeUI/gettutorial.png",				//	アイテムとのやり取り
		"data/TEXTURE/gamemodeUI/changeweapontutorial.png",		//　武器の切り替え
		"data/TEXTURE/gamemodeUI/reloadtutorial.png",			//　リロード

};


static GMUI g_GMUI[UI_MAX];

static float uiTextureWidthList[UI_MAX] = {
	UI_CROSS_WIDTH,		//	照準
	UI_GET_WIDTH,		//	入手	
	UI_GET_WIDTH,		//  箱を移動させる
	UI_GET_WIDTH,		//	ドアなど開く
	UI_HANDGUN_WIDTH,	//	ピストル
	UI_HANDGUN_WIDTH,	//	武器なしのアイコン
	UI_RELOAD_WIDTH,	//　リロードのBG
	UI_RELOAD_WIDTH,	//　リロードの真ん中の部分
	UI_RELOAD_WIDTH,	//  リロードのテキスト
	UI_LOCK_WIDTH,		//	ドアがロックされているテキスト
	UI_GET_WIDTH,		//	ドアなど閉じる
	//チュートリアル用
	UI_TUTORIAL_WIDTH,	//　移動
	UI_TUTORIAL_WIDTH,	//	ジャンプ
	UI_TUTORIAL_WIDTH,	//	アイテムとのやり取り
	UI_TUTORIAL_WIDTH,	//　武器の切り替え
	UI_TUTORIAL_WIDTH,	//　リロード
};

static float uiTextureHeightList[UI_MAX] = {
	UI_CROSS_HEIGHT,   //	照準
	UI_GET_HEIGHT,	   //	入手	
	UI_GET_HEIGHT,	   //  箱を移動させる
	UI_GET_HEIGHT,	   //	ドアなど開く
	UI_HANDGUN_HEIGHT, //	ピストル
	UI_HANDGUN_HEIGHT, //	武器なしのアイコン
	UI_RELOAD_HEIGHT,  //　リロードのBG
	UI_RELOAD_HEIGHT,  //　リロードの真ん中の部分
	UI_RELOAD_HEIGHT,  //  リロードのテキスト
	UI_LOCK_HEIGHT,	   //	ドアがロックされているテキスト
	UI_GET_HEIGHT,     //	ドアなど閉じる
	//チュートリアル用
	UI_TUTORIAL_HEIGHT,//　移動
	UI_TUTORIAL_HEIGHT,//	ジャンプ
	UI_TUTORIAL_HEIGHT,//	アイテムとのやり取り
	UI_TUTORIAL_HEIGHT,//　武器の切り替え
	UI_TUTORIAL_HEIGHT,//　リロード
};

static XMFLOAT3 uiTexturePositionList[UI_MAX] = {
	UI_CROSS_POSITION,			//	照準
	UI_GET_POSITION,			//	入手	
	UI_GET_POSITION,			//  箱を移動させる
	UI_GET_POSITION,			//	ドアなど開く
	UI_HANDGUN_POSITION,		//	ピストル
	UI_WEAPON_NONE_POSITION,	//	武器なしのアイコン
	UI_RELOAD_BG_POSITION,		//　リロードのBG
	UI_RELOAD_BG_POSITION,		//　リロードの真ん中の部分
	UI_RELOAD_TEXT_POSITION,	//  リロードのテキスト
	UI_LOCK_POSITION,			//	ドアがロックされているテキスト
	UI_GET_POSITION,			//	ドアなど閉じる
	//チュートリアル用
	UI_TUTORIAL_POS,			//　移動
	UI_TUTORIAL_POS2,			//	ジャンプ
	UI_TUTORIAL_POS,			//	アイテムとのやり取り
	UI_TUTORIAL_POS,			//　武器の切り替え
	UI_TUTORIAL_POS,			//　リロード
};

static bool g_CanDrawUI = true;

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
	g_GMUI[UI_HANDGUN].use = false;
	g_GMUI[UI_WEAPON_NONE].diff = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.9f);

	g_GMUI[UI_RELOAD_BG].use = false;
	g_GMUI[UI_RELOAD_FILL].use = false;
	g_GMUI[UI_RELOAD_TEXT].use = false;

	g_GMUI[UI_DOOR_LOCK_TEXT].use = false;

	g_GMUI[UI_GET].use = false;
	g_GMUI[UI_OPEN].use = false;
	g_GMUI[UI_MOVE].use = false;
	g_GMUI[UI_CLOSE].use = false;

	g_GMUI[UI_GET_TUTORIAL].use = false;
	g_GMUI[UI_CHANGEWEAPON_TUTORIAL].use = false;
	g_GMUI[UI_RELOAD_TUTORIAL].use = false;




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
	// メッセージが一定の時間に表示されたら　消える
	if (g_GMUI[UI_DOOR_LOCK_TEXT].use) {
		g_GMUI[UI_DOOR_LOCK_TEXT].count++;
		if (g_GMUI[UI_DOOR_LOCK_TEXT].count >= 50) {
			g_GMUI[UI_DOOR_LOCK_TEXT].count = 0;
			g_GMUI[UI_DOOR_LOCK_TEXT].use = false;
		}
	}


	

#ifdef _DEBUG	// デバッグ情報を表示する

	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGMUI(void)
{
	if (g_CanDrawUI) {
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
					// UI_FILL の場合
					if (i == UI_RELOAD_FILL) {
						SetSpriteLeftTopColor(g_VertexBuffer,
							g_GMUI[i].pos.x - uiTextureWidthList[i] * 0.5f,
							g_GMUI[i].pos.y - uiTextureHeightList[i] * 0.5f,
							g_GMUI[i].width, g_GMUI[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_GMUI[i].diff);
					}
					//	その他の場合
					else {
						SetSpriteColor(g_VertexBuffer, g_GMUI[i].pos.x, g_GMUI[i].pos.y, g_GMUI[i].width, g_GMUI[i].height, 0.0f, 0.0f, 1.0f, 1.0f, g_GMUI[i].diff);
					}

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}
			}
		}

	}
	


}




//　指定されたUIの表示のON/OFF
void TurnOnOffUI(int num,bool onoff) {
	g_GMUI[num].use = onoff;
}

// UIのアルファ値を変更する関数
void ChangeUIDiff(int num, XMFLOAT4 diff) {
	g_GMUI[num].diff = diff;
}

// UIのwidth値を変更する関数
void ChangeUIWidth(int num, float scl) {
	g_GMUI[num].width = uiTextureWidthList[num] * scl;
}

// 指定されたUIのdiff値を得る
XMFLOAT4 GetUIDiff(int num) {
	return g_GMUI[num].diff;
}

// リロードのUIのON/OFF
void TurnReloadUIOnOff(bool status) {
	g_GMUI[UI_RELOAD_BG].use = status;
	g_GMUI[UI_RELOAD_FILL].use = status;
	g_GMUI[UI_RELOAD_TEXT].use = status;

	g_GMUI[UI_RELOAD_BG].diff.w = 1.0f;
	g_GMUI[UI_RELOAD_FILL].diff.w = 1.0f;
	g_GMUI[UI_RELOAD_TEXT].diff.w = 1.0f;

}

void TurnOnOffUIAll(bool OnOff) {
	g_CanDrawUI = OnOff;
}

bool GetCanDrawUI() {
	return g_CanDrawUI;
}

void CheckPlayerTutorialStage(void) {
	PLAYER* player = GetPlayer();
	if (player->moveStage == false && player->changeWeaponStage == false && player->reloadStage == false) {

	}
}