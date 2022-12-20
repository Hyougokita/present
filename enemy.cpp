//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(17.0f)						// エネミーの足元をあわせる

#define MODE_BERA_BODY			"data/MODEL/bear_body.obj"
#define MODE_BERA_RIGHT_HAND	"data/MODEL/bear_righthand.obj"
#define MODE_BERA_LEFT_HAND		"data/MODEL/bear_lefthand.obj"
#define MODE_BEAR_HEAD			"data/MODEL/bear_head.obj"
#define MODE_BEAR_RIGHT_LEG		"data/MODEL/bear_rightleg.obj"
#define MODE_BEAR_LEFT_LEG		"data/MODEL/bear_leftleg.obj"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void EnemyMoveForward(ENEMY* enemy, XMFLOAT3 vec);
XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー
static ENEMY_PARTS		g_EnemyParts[MAX_ENEMY];		// エネミー　パーツ
static BOOL				g_Load = FALSE;

enum ENEMY_ANIM_TYPE
{
	ENEMY_IDLE,
	ENEMY_MOVE
};

//　モデルのファイルパス
char* modelPath[5] =
{ MODE_BERA_RIGHT_HAND,
	MODE_BERA_LEFT_HAND,
	MODE_BEAR_HEAD,
	MODE_BEAR_LEFT_LEG,
	MODE_BEAR_RIGHT_LEG,
};



// 歩くの線形補間
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.5f },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },

};

static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_right_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_left_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),				XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),  30},
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(-XM_PI / 8, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 8, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};



// IDLEの線形補間
static INTERPOLATION_DATA idle_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,	ENEMY_OFFSET_Y,  20.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.5f },
	{ XMFLOAT3(0.0f,	ENEMY_OFFSET_Y,  20.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2 },

};

static INTERPOLATION_DATA idle_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -XM_PI / 8), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_right_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI / 8),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_left_leg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, -0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

static INTERPOLATION_DATA idle_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),					XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI / 16, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 16, 0.0f),			XMFLOAT3(1.0f, 1.0f, 1.0f), 15 },

};

// 歩くのアニメーシ
static INTERPOLATION_DATA* move_tblAdr[] = {
	move_tbl_right,
	move_tbl_left,
	move_tbl_head,
	move_tbl_left_leg,
	move_tbl_right_leg,
};


// IDLEのアニメーシ
static INTERPOLATION_DATA* idle_tblAdr[] = {
	idle_tbl_right,
	idle_tbl_left,
	idle_tbl_head,
	idle_tbl_left_leg,
	idle_tbl_right_leg,
};

//　moveのアニメーシのサイズ
static int move_tblSize[MAX_ENMEY_PARTS] = {
	sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_left_leg) / sizeof(INTERPOLATION_DATA),
	sizeof(move_tbl_right_leg) / sizeof(INTERPOLATION_DATA),
};

//　idleのアニメーシのサイズ
static int idle_tblSize[MAX_ENMEY_PARTS] = {
	sizeof(idle_tbl_right) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_left) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_head) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_left_leg) / sizeof(INTERPOLATION_DATA),
	sizeof(idle_tbl_right_leg) / sizeof(INTERPOLATION_DATA),
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	// 本体
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODE_BERA_BODY, &g_Enemy[i].model);
		g_Enemy[i].load = true;
		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.2f, 0.2f, 0.2f);
		g_Enemy[i].spd = 1.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].Parent = NULL;

		g_Enemy[i].actType = ENEMY_IDLE;

		g_Enemy[i].use = true;			// true:生きてる

		// これから　パーツの処理
		for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
			// モデルの読み込み
			LoadModel(modelPath[j], &g_EnemyParts[i].Parts[j].model);
			g_EnemyParts[i].Parts[j].load = true;

			//　位置　回転　拡大縮小
			g_EnemyParts[i].Parts[j].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
			g_EnemyParts[i].Parts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyParts[i].Parts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			//　線形補間
			g_EnemyParts[i].Parts[j].move_time = 0.0f;
			g_EnemyParts[i].Parts[j].tbl_adr = idle_tblAdr[j];
			g_EnemyParts[i].Parts[j].tbl_size = idle_tblSize[j];
			//　生きているかどうか
			g_EnemyParts[i].Parts[j].use = true;

			//	親子関係
			g_EnemyParts[i].Parts[j].Parent = &g_Enemy[i];
		}
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER *player = GetPlayer();
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == true)			// このエネミーが使われている？
		{									// Yes
			if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
			{								// 線形補間の処理
				// 移動処理
				int		index = (int)g_Enemy[i].move_time;
				float	time = g_Enemy[i].move_time - index;
				int		size = g_Enemy[i].tbl_size;

				float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
				g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					g_Enemy[i].move_time = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			}
			else {
				XMFLOAT3 vec = MakeUnitVector(g_Enemy[i].pos, player->pos);
				EnemyMoveForward(&g_Enemy[i], vec);
			}

			
			// レイキャストして足元の高さを求める
			XMFLOAT3 HitPosition;		// 交点
			XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
			bool ans = RayHitField(g_Enemy[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Enemy[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}


			// 影もエネミーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}

	/*パーツ*/

	//　本体の数
	for (int i = 0; i < MAX_ENEMY; i++) {
		//　パーツの数
		for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
			// パーツが生きている場合
			if (g_EnemyParts[i].Parts[j].use) {
				/*行動状態の判断*/

				// IDLEのとき
				if (g_Enemy[i].actType == ENEMY_IDLE) {
					for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
						g_EnemyParts[i].Parts[j].tbl_adr = idle_tblAdr[j];
						g_EnemyParts[i].Parts[j].tbl_size = idle_tblSize[j];
					}
				}
				else if (g_Enemy[i].actType == ENEMY_MOVE) {
					for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
						g_EnemyParts[i].Parts[j].tbl_adr = move_tblAdr[j];
						g_EnemyParts[i].Parts[j].tbl_size = move_tblSize[j];
					}
				}

				{	// 線形補間の処理
					int nowNo = (int)g_EnemyParts[i].Parts[j].move_time;		// 整数分であるテーブル番号を取り出している
					int maxNo = g_EnemyParts[i].Parts[j].tbl_size;				// 登録テーブル数を数えている
					int nextNo = (nowNo + 1) % maxNo;							// 移動先テーブルの番号を求めている
					INTERPOLATION_DATA* tbl = g_EnemyParts[i].Parts[j].tbl_adr;	// 行動テーブルのアドレスを取得

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

					float nowTime = g_EnemyParts[i].Parts[j].move_time - nowNo;	// 時間部分である少数を取り出している

					Pos *= nowTime;								// 現在の移動量を計算している
					Rot *= nowTime;								// 現在の回転量を計算している
					Scl *= nowTime;								// 現在の拡大率を計算している

					// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
					XMStoreFloat3(&g_EnemyParts[i].Parts[j].pos, nowPos + Pos);

					// 計算して求めた回転量を現在の移動テーブルに足している
					XMStoreFloat3(&g_EnemyParts[i].Parts[j].rot, nowRot + Rot);

					// 計算して求めた拡大率を現在の移動テーブルに足している
					XMStoreFloat3(&g_EnemyParts[i].Parts[j].scl, nowScl + Scl);

					// frameを使て時間経過処理をする

					g_EnemyParts[i].Parts[j].move_time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
					if ((int)g_EnemyParts[i].Parts[j].move_time >= maxNo)			// 登録テーブル最後まで移動したか？
					{
						g_EnemyParts[i].Parts[j].move_time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
					}
				}
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する

	// キーKを押すと移動状態に変更する
	if (GetKeyboardTrigger(DIK_K)) {
		for (int i = 0; i < MAX_ENEMY; i++) {
			g_Enemy[i].actType = ENEMY_MOVE;
		}
	}

	// キーJを押すとIDLE状態に変更する
	if (GetKeyboardTrigger(DIK_J)) {
		for (int i = 0; i < MAX_ENEMY; i++) {
			g_Enemy[i].actType = ENEMY_IDLE;
		}
	}

#endif
}



//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Enemy[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);



		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);
		for (int j = 0; j < MAX_ENMEY_PARTS; j++) {
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_EnemyParts[i].Parts[j].scl.x, g_EnemyParts[i].Parts[j].scl.y, g_EnemyParts[i].Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyParts[i].Parts[j].rot.x, g_EnemyParts[i].Parts[j].rot.y, g_EnemyParts[i].Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_EnemyParts[i].Parts[j].pos.x, g_EnemyParts[i].Parts[j].pos.y, g_EnemyParts[i].Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (&g_EnemyParts[i].Parts[j].Parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_EnemyParts[i].Parts[j].Parent->mtxWorld));
				// ↑
				// g_Player.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_EnemyParts[i].Parts[j].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_EnemyParts[i].Parts[j].use == false) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_EnemyParts[i].Parts[j].model);
		}
	}



	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//　エネミーをある方向に移動させる
void EnemyMoveForward(ENEMY *enemy, XMFLOAT3 vec) {
	enemy->pos.x += vec.x * enemy->spd;
	enemy->pos.z += vec.z * enemy->spd;

	//姿勢制御
	XMFLOAT3 Normal;
	Normal = { -vec.x,0.0f,vec.z };

	XMVECTOR vx, nvx, front;
	XMVECTOR quat;
	float len, angle;


	enemy->XVector = Normal;

	//front = { 0.0f, 0.0f, 1.0f, 0.0f };
	if (vec.z > 0) {
		enemy->rot.y = XMConvertToRadians(180.0f);
		front = { 0.0f, 0.0f, -1.0f, 0.0f };
	}
	else {
		enemy->rot.y = XMConvertToRadians(0.0f);
		front = { 0.0f, 0.0f, 1.0f, 0.0f };
	}


	vx = XMVector3Cross(front, XMLoadFloat3(&enemy->XVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);

	angle = asinf(len);

	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&enemy->Quaternion), quat, 0.05f);
	XMStoreFloat4(&enemy->Quaternion, quat);
}


// 両点の単位ベクトルを求める
XMFLOAT3 MakeUnitVector(XMFLOAT3 start, XMFLOAT3 end) {
	XMFLOAT3 vec = XMFLOAT3(0.0f,0.0f,0.0f);
	float disX = end.x - start.x;
	float disZ = end.z - start.z;

	float distance = pow((pow(disX, 2) + pow(disZ, 2)), 0.5);
	vec.x = disX / distance;
	vec.z = disZ / distance;


	return vec;
}
