//=============================================================================
//
//
//=============================================================================
#pragma once

#include "main.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "result.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/

/*******************************************************************************
* 構造体定義
*******************************************************************************/
struct	SAVEDATA				// セーブデータの構造体
{

	// ハイスコアデータを保存
	int		highScore[HIGHSCORE_MAX];



};



/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
void LoadHighscore(void);
void SaveHighscore(void);

