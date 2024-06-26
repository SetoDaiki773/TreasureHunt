#pragma once
#include "d3dx12.h"
#include <directXmath.h>
#include <vector>




namespace Math {

	

	//---------------------------------------------------------------
	/// [機能] 二つの引数がほぼ同じならtrueを返します(foat用)
	/// [引数1] 比べたい数値です
	/// [引数2] 引数1と比べる数値です
	/// [返り値] 引数1と引数2がほぼ同じならtrueを返します
	//---------------------------------------------------------------
	bool NearEqualF(float num1, float num2);

	//----------------------------------------------------------------
	/// [機能] 桁数を取得する
	/// [引数] 桁数を取得したい数値
	/// [戻り値] 桁数
	//----------------------------------------------------------------
	int NumDigits(int num);

	//----------------------------------------------------------------
	/// [機能] 指定した桁の数値を取得する
	/// [引数1] 抽出元の数値
	/// [引数2] 数値を取得したい桁数
	/// [戻り値] 指定された桁の数値
	//----------------------------------------------------------------
	int GetNumOfDigit(int num, int digit);
 
}
