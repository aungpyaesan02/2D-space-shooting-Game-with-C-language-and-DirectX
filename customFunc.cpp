//=============================================================================
// ファイル：汎用的な関数をまとめたファイル [customFunc.cpp]
// 製作者：GP11A132 11 小峰遥一
//=============================================================================
#include "main.h"
#include "customFunc.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// 指定範囲内のランダムな値を取得する関数
// ---------------------------------------------------------------------------
//【引数】
// 最小値	min
// 最大値	max
//=============================================================================
int RandomRange(int min, int max)
{
	return rand() % (max - min + 1) + min;
}


//=============================================================================
// 値を最小・最大に収まるようにする関数
// ---------------------------------------------------------------------------
//【引数】
// 値		num
// 最小値	min
// 最大値	max
//=============================================================================
float Clamp(float num, float min, float max)
{
	if (num < min)
	{
		num = min;
	}
	else if (num > max)
	{
		num = max;
	}

	return num;
}


//=============================================================================
// 線形補間関数
// ---------------------------------------------------------------------------
//【引数】
// 始値		start
// 終値		end
// 変化率	t
//=============================================================================
float Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}


//=============================================================================
// イージング関数１（途中までほぼ０、そこから一気に１へ）
// ---------------------------------------------------------------------------
//【引数】
// 変化率（０〜１）	a
//=============================================================================
float EaseInQuint(float a)
{
	return a * a * a * a * a;
}