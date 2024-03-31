#include "Random.h"
#include <stdlib.h>
#include <time.h>
#include <cassert>

unsigned int Random::mSeed = -1;

//-----------------------------------------
//    初期化
//-----------------------------------------
void Random::Init() {
	mSeed = (unsigned int)time(NULL);   // ランダムのシード値設定
	srand(mSeed);
}

//-------------------------------------------
//   min～maxの間のランダムな整数値を返す
//-------------------------------------------
int Random::RandomInt(int min,int max) {
	if (mSeed == -1) {
		Init();
	}
	assert((min <= max) || !("RandomInt: minがmaxより大きいです"));   // minがmax以下が偽ならasset

	return (rand() % (max - min + 1) + min);  // min～maxの範囲から返す
}


//-------------------------------------------------
//    min～maxでランダムな浮動小数点数を返す
//-------------------------------------------------
float Random::RandomFloat(int min,int max) {
	if (mSeed == -1) {
		Init();
	}
	assert(min <= max || !("RandomFloat: minがmaxより大きいです"));   // minがmax以下が偽ならassret

	int floatRange = 0x7fff;                                              // ある程度正確な精度の小数点以下数値の最大値
	float decimalPart = (rand() % (floatRange + 1) / (float)floatRange);  // 小数部の数値設定
	int integerPart = (rand() % (max - min ) + min);                      // 整数部の数値設定
	return decimalPart + static_cast<float>(integerPart);                 // 整数部に小数部を足して返す
}


//----------------------------------------------------------------------------
//   min～maxでランダムな浮動小数点数を返す。引数が浮動小数点数バージョン
//----------------------------------------------------------------------------
float Random::RandomFloat(float min,float max) {
	if (mSeed == -1) {
		Init();
	}
	assert(min <= max || !("RandomFloat: minがmaxより大きいです"));                        // minがmax以下が偽ならassret
	int maxInt = static_cast<int>(max);
	int minInt = static_cast<int>(min);
	int floatRange = 0x7fff;                                                               // ある程度正確な精度の小数点以下数値の最大値
	float returnNum = 0.0f;
	do {
		int integerPart = 0;
		if (maxInt != 0) {
			integerPart = (rand() % (maxInt - minInt) + minInt);                           // 整数部の数値設定
		}
		else {
			integerPart = 0;
		}
		float decimalPart = (rand() % (floatRange + 1) / static_cast<float>(floatRange));  // 小数部の数値設定
		returnNum = static_cast<float>(integerPart) + decimalPart;
	} while (returnNum <= min || returnNum >= max);                                        // 最大値を超えたり、最小値を下回るならやり直し
	
	return returnNum;

}
