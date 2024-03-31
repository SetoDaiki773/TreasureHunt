#include "MyMath.h"
#include "VectorOperation.h"
#include <cmath>


using namespace DirectX;


//----------------------------------------
//   二つの数値がほぼ同じか調べる
//----------------------------------------
bool Math::NearEqualF(float num1,float num2) {
	float error = num1 - num2;  // 誤差の取得
	if (error < 0.1f && error > -0.1f) {  // 誤差0.01未満なら
		return true;  // trueを返す
	}
	return false; // 誤差が0.01より大きいならfalseを返す
}


//-----------------------------------------
//  桁数を取得する
//-----------------------------------------
int Math::NumDigits(int num) {
	if (num == 0) return 1;
	return static_cast<int>(std::log10(num) + 1);
}

//-----------------------------------------
// 指定した桁の数値を取得する
//-----------------------------------------
int Math::GetNumOfDigit(int num, int digit) {
	assert(num >= 0);
	if(NumDigits(num) < digit) return 0;  // numの桁数が指定された桁よりも少ないなら0を還す
	int divisor = static_cast<int>(std::pow(10, digit));
	int numUpToThisDigit = (num % divisor);
	int numOfThisDigit = numUpToThisDigit / static_cast<int>((std::pow(10, digit - 1)));
	return numOfThisDigit;
}
