#include "MyMath.h"
#include "VectorOperation.h"
#include <cmath>


using namespace DirectX;


//----------------------------------------
//   ��̐��l���قړ��������ׂ�
//----------------------------------------
bool Math::NearEqualF(float num1,float num2) {
	float error = num1 - num2;  // �덷�̎擾
	if (error < 0.1f && error > -0.1f) {  // �덷0.01�����Ȃ�
		return true;  // true��Ԃ�
	}
	return false; // �덷��0.01���傫���Ȃ�false��Ԃ�
}


//-----------------------------------------
//  �������擾����
//-----------------------------------------
int Math::NumDigits(int num) {
	if (num == 0) return 1;
	return static_cast<int>(std::log10(num) + 1);
}

//-----------------------------------------
// �w�肵�����̐��l���擾����
//-----------------------------------------
int Math::GetNumOfDigit(int num, int digit) {
	assert(num >= 0);
	if(NumDigits(num) < digit) return 0;  // num�̌������w�肳�ꂽ���������Ȃ��Ȃ�0���҂�
	int divisor = static_cast<int>(std::pow(10, digit));
	int numUpToThisDigit = (num % divisor);
	int numOfThisDigit = numUpToThisDigit / static_cast<int>((std::pow(10, digit - 1)));
	return numOfThisDigit;
}
