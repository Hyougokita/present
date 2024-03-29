//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(30.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-30.0f)		// カメラの初期位置(Z座標)

//#define	POS_X_CAM		(0.0f)			// カメラの初期位置(X座標)
//#define	POS_Y_CAM		(200.0f)		// カメラの初期位置(Y座標)
//#define	POS_Z_CAM		(-400.0f)		// カメラの初期位置(Z座標)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(0.1f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(10000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE_CAMERA	(2.0f)										// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.005f)								// カメラの回転量

bool g_LockMouse = true;
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera;		// カメラデータ

static int				g_ViewPortType = TYPE_FULL_SCREEN;

// マウス関連
tagPOINT lpPoint;
XMFLOAT2 g_MousePos = XMFLOAT2(0.0f, 0.0f);
//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at  = { 0.0f, 0.0f, 0.0f };
	g_Camera.up  = { 0.0f, 1.0f, 0.0f };
	g_Camera.rot = { 0.0f, 0.0f, 0.0f };
	g_Camera.upDown = 0.0f;

	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);
	
	// ビューポートタイプの初期化
	g_ViewPortType = TYPE_FULL_SCREEN;
}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{

	if (GetMode() == MODE_GAME || GetMode() == MODE_TUTORIAL) {
		ShowCursor(false);

		//　マウスの位置をスクリーンの真ん中に固定する
		int ScreenMidX = GetSystemMetrics(SM_CXSCREEN) / 2;
		int ScreenMidY = GetSystemMetrics(SM_CYSCREEN) / 2;
		if (g_LockMouse) {
			SetCursorPos(ScreenMidX, ScreenMidY);
		}

		//	マウスロックの解錠
		if (GetKeyboardTrigger(DIK_X)) {
			g_LockMouse = false ? g_LockMouse : g_LockMouse == false;
		}


		//	現在のマウスの位置を取得する
		GetCursorPos(&lpPoint);
		g_MousePos.x = (float)lpPoint.x;
		g_MousePos.y = (float)lpPoint.y;

		if ((int)GetMouseX() + 8.0f< SCREEN_CENTER_X)
		{// 視点旋回「左」
			g_Camera.rot.y += VALUE_ROTATE_CAMERA;
			if (g_Camera.rot.y > XM_PI)
			{
				g_Camera.rot.y -= XM_PI * 2.0f;
			}

			g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
			g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
		}

		if ((int)GetMouseX() - 8.0f > SCREEN_CENTER_X)
		{// 視点旋回「上」
			g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
			if (g_Camera.rot.y < -XM_PI)
			{
				g_Camera.rot.y += XM_PI * 2.0f;
			}

			g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
			g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
		}

		if ((int)GetMouseY() + 5.0f < 240.0f)
		{// 視点旋回「下」
			g_Camera.upDown += 1.0f;

			if (g_Camera.upDown >= 50.0f) {
				g_Camera.upDown = 50.0f;
			}
		}

		if ((int)GetMouseY() - 5.0f > 240.0f)
		{// 視点旋回「右」
			g_Camera.upDown -= 1.0f;

			if (g_Camera.upDown <= -50.0f) {
				g_Camera.upDown = -50.0f;
			}
		}


	}

	if (GetKeyboardPress(DIK_Z))
	{// 視点旋回「左」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_C))
	{// 視点旋回「右」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// 視点移動「上」
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// 視点移動「下」
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// 注視点旋回「左」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	//if (GetKeyboardPress(DIK_E))
	//{// 注視点旋回「右」
	//	g_Camera.rot.y += VALUE_ROTATE_CAMERA;
	//	if (g_Camera.rot.y > XM_PI)
	//	{
	//		g_Camera.rot.y -= XM_PI * 2.0f;
	//	}

	//	g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
	//	g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	//}

	if (GetKeyboardPress(DIK_T))
	{// 注視点移動「上」
		g_Camera.at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// 注視点移動「下」
		g_Camera.at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// 近づく
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

#ifdef _DEBUG
	// カメラを初期に戻す
	//if (GetKeyboardPress(DIK_R))
	//{
	//	UninitCamera();
	//	InitCamera();
	//}
#endif



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Camera:ZC QE TB YN UM\n");
	PrintDebugProc("MouseCurrentPos: %f %f\n", g_MousePos.x, g_MousePos.y);
	PrintDebugProc("Camera Rot X:%f\n", g_Camera.rot.x);
	PrintDebugProc("Camera Updown %f\n", g_Camera.upDown);
	PrintDebugProc("Camera Pos X:%f,Y:%f,Z:%f\n", g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z);
	PrintDebugProc("Camera Length:%f\n", g_Camera.len);
	PrintDebugProc("Mouse lock:%d\n", g_LockMouse);
#endif
}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void) 
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

//=============================================================================
// ビューポートの設定
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// ビューポート設定
	switch (g_ViewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return g_ViewPortType;
}



// カメラの視点と注視点をセット
void SetCameraAT(XMFLOAT3 pos)
{
	// カメラの注視点をプレイヤーの座標にしてみる
	g_Camera.at = pos;

	// カメラの視点をカメラのY軸回転に対応させている
	//g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
	//g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	g_Camera.pos.x = GetPlayer()->pos.x;
	g_Camera.pos.z = GetPlayer()->pos.z;
	g_Camera.pos.y = GetPlayer()->pos.y + PLAYER_HEAD_HEIGHT;
										//↑頭の高さ
}

void SetCameraAT2(XMFLOAT3 pos)
{
	// カメラの注視点をプレイヤーの座標にしてみる
	g_Camera.at = pos;

	// カメラの視点をカメラのY軸回転に対応させている
	//g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
	//g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;

	//g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * 200.0f;
	//g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * 200.0f;
	g_Camera.pos = XMFLOAT3(316.0f, 100.0f, -211.0f);
}

