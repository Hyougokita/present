//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	BULLET_WIDTH		(5.0f)			// 頂点サイズ
#define	BULLET_HEIGHT		(5.0f)			// 頂点サイズ

#define	BULLET_SPEED		(5.0f)			// 弾の移動スピード

#define	HANDGUN_CUR_MAX		(15)			//　ピストルの一つのマガジンの上限

#define DEBUG
//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BULLET						g_Bullet[MAX_BULLET];	// 木ワーク
static int							g_TexNo;				// テクスチャ番号
static BOOL							g_Load = FALSE;

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/bullet01.png",
};

int g_CurMagazine = 0;	//　現在の弾の数
int g_SubMagazine = 0;	//	後備の弾の数

//　ピストルの現在と後備弾薬
int g_HandGunCurMagazine = 0;
int g_HandGunSubMagazine = 20;

static int curMagazineList[WEAPON_MAX] = {
	0,
	g_HandGunCurMagazine,
};

static int subMagazineList[WEAPON_MAX] = {
	0,
	g_HandGunSubMagazine,
};

static int maxMagazineList[WEAPON_MAX] = {
	0,
	HANDGUN_CUR_MAX
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	MakeVertexBullet();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_Bullet[nCntBullet].material, sizeof(g_Bullet[nCntBullet].material));
		g_Bullet[nCntBullet].material.Diffuse = { 1.0f, 0.0f, 0.0f, 1.0f };

		g_Bullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_Bullet[nCntBullet].angleY = 0.0f;
		g_Bullet[nCntBullet].spd = BULLET_SPEED;
		g_Bullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_Bullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_Bullet[nCntBullet].use = false;
#ifdef DEBUG
		g_Bullet[nCntBullet].count = 0;
#endif // DEBUG


	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{
	// プレーヤーが手にあるウェポンにより弾表示の切り替え
	int weaponType = GetPlayer()->curWeapon;
	g_CurMagazine = curMagazineList[weaponType];
	g_SubMagazine = subMagazineList[weaponType];


	CAMERA* camera = GetCamera();
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// 弾の移動処理
			g_Bullet[i].pos.x += sinf(g_Bullet[i].rot.y) * g_Bullet[i].spd;
			g_Bullet[i].pos.z -= cosf(g_Bullet[i].rot.y) * g_Bullet[i].spd;
			g_Bullet[i].pos.y += sinf(g_Bullet[i].angleY) * g_Bullet[i].spd;
			//g_Bullet[i].pos.y += cosf(XM_PI / 4) * g_Bullet[i].spd;


			// 影の位置設定
			SetPositionShadow(g_Bullet[i].shadowIdx, XMFLOAT3(g_Bullet[i].pos.x, 0.1f, g_Bullet[i].pos.z));


			// フィールドの外に出たら弾を消す処理
			if (g_Bullet[i].pos.x < MAP_LEFT
				|| g_Bullet[i].pos.x > MAP_RIGHT
				|| g_Bullet[i].pos.z < MAP_DOWN
				|| g_Bullet[i].pos.z > MAP_TOP)
			{
				g_Bullet[i].use = false;
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}
#ifdef DEBUG
			if (g_Bullet[i].count >= 100) {
				g_Bullet[i].use = false;
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}

#endif // DEBUG

		}
	}

#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Bullet X:%f Y:%f Z:%f\n", g_Bullet[0].pos.x, g_Bullet[0].pos.y, g_Bullet[0].pos.z);
	PrintDebugProc("Bullet CameraAT Y:%f CameraPos Y:%f\n", GetCamera()->at.y,GetCamera()->pos.y);
	PrintDebugProc("Cur:%d,Sub:%d\n", g_CurMagazine, g_SubMagazine);

#endif

}




//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// ライティングを無効
	SetLightEnable(false);
	


	// αテストを有効に
	SetAlphaTestEnable(true);

	//SetBlendState(BLEND_MODE_ADD);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, XM_PI-g_Bullet[i].rot.y , g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_Bullet[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetAlphaTestEnable(false);
	//SetBlendState(BLEND_MODE_ALPHABLEND);
	// ライティングを有効に
	SetLightEnable(true);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBullet(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f, fHeight / 2.0f,  0.0f };
	vertex[1].Position = {  fWidth / 2.0f, fHeight / 2.0f,  0.0f };
	vertex[2].Position = { -fWidth / 2.0f, -fHeight / 2.0f, 0.0f };
	vertex[3].Position = {  fWidth / 2.0f, -fHeight / 2.0f, 0.0f };

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{
	if (curMagazineList[GetPlayer()->curWeapon] <= 0)
		return -1;

	curMagazineList[GetPlayer()->curWeapon] -= 1;

	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			g_Bullet[nCntBullet].pos = pos;
			g_Bullet[nCntBullet].rot = rot;
			g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_Bullet[nCntBullet].angleY = AngleY();

			g_Bullet[nCntBullet].pos.x += sinf(g_Bullet[nCntBullet].rot.y) *  g_Bullet[nCntBullet].spd * 5;
			g_Bullet[nCntBullet].pos.z -= cosf(g_Bullet[nCntBullet].rot.y) *  g_Bullet[nCntBullet].spd * 5;
			g_Bullet[nCntBullet].pos.y += sinf(g_Bullet[nCntBullet].angleY) * g_Bullet[nCntBullet].spd * 5;

			g_Bullet[nCntBullet].use = true;

			// 影の設定
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// 弾の取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

//  垂直と水平線の角度を求める
float AngleY() {
	float angle = 0.0f;


	PLAYER* player = GetPlayer();
	CAMERA* camera = GetCamera();

	float DistanceY = camera->at.y - camera->pos.y;
	float DistanceX = camera->len;

	angle = atanf(DistanceY/DistanceX);


	return angle;
}

// 現在の弾の数を得る
int GetCurMagazine() {
	return g_CurMagazine;
}
// 後備の弾の数を得る
int GetSubMagazine() {
	return g_SubMagazine;
}

//	後備の弾の数を増やす
void AddSubMagazine(int num, int type) {
	if(type != WEAPON_NONE)
	subMagazineList[type] += num;
}

//　リロード
void Reload(int type) {
	if (type != WEAPON_NONE) {
		//　リロード必要の弾数を計算する
		int need = maxMagazineList[type] - curMagazineList[type];
		subMagazineList[type] -= need;

		//	後備弾薬の数が足りない場合
		if (subMagazineList[type] <= 0) {
			//　残る後備弾薬を全部ロードする
			curMagazineList[type] = subMagazineList[type] + maxMagazineList[type];
			subMagazineList[type] = 0;
		}
		//　後備弾薬が足りる場合
		else
		{
			//　ロードする
			curMagazineList[type] = maxMagazineList[type];
		}
	}
}