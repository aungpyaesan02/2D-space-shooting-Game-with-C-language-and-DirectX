//=============================================================================
//

//
//=============================================================================
#include "main.h"
#include "collision.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// BBによる当たり判定処理(回転は考慮しない)
// ----------------------------------------------------------------------------
// 【戻り値】
// 当たってたらTRUE
//=============================================================================
BOOL CollisionBB(D3DXVECTOR3 mpos, float mw, float mh,
				 D3DXVECTOR3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BCによる当たり判定処理
// ----------------------------------------------------------------------------
// 【引数】
// pos1, pos2			オブジェクトの座標
// r1, r2				オブジェクトの半径
//
// 【戻り値】
// 当たってたらTRUE
//=============================================================================
BOOL CollisionBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;

	float len = (r1 + r2) * (r1 + r2);		// 半径を2乗した距離
	D3DXVECTOR3 temp = pos1 - pos2;
	float lenSq = D3DXVec3LengthSq(&temp);	// 2点間の距離（2乗した物）

	// 半径を2乗した物より距離が短い？
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}