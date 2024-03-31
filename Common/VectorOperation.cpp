#include "VectorOperation.h"
#include <cmath>
#include <iostream>

using namespace DirectX;



//------------------------------
//   �x�N�g���̉��Z
//------------------------------
const XMFLOAT3 operator+(const XMFLOAT3& vector1, const XMFLOAT3& vector2) {
	return XMFLOAT3(
	vector1.x + vector2.x, 
	vector1.y + vector2.y, 
	vector1.z + vector2.z
	);
}

const XMFLOAT3 operator+=(const XMFLOAT3& vector1, const XMFLOAT3& vector2) {
	return XMFLOAT3(
	vector1.x + vector2.x,
	vector1.y + vector2.y,
	vector1.z + vector2.z
	);
}

//-------------------------------
//  �x�N�g���̌��Z
//-------------------------------
const XMFLOAT3 operator-(const XMFLOAT3& vector1, const XMFLOAT3& vector2) {
	return XMFLOAT3(
	vector1.x - vector2.x,
	vector1.y - vector2.y,
	vector1.z - vector2.z
	);
}

const XMFLOAT3 operator-=(const XMFLOAT3& vector1,const XMFLOAT3& vector2) {
	return XMFLOAT3(
	vector1.x - vector2.x,
	vector1.y - vector2.y,
	vector1.z - vector2.z
	);
}

//--------------------------------
//   �x�N�g���ƃX�J���[�̏�Z
//--------------------------------
const DirectX::XMFLOAT3 operator*(float scaler, const DirectX::XMFLOAT3& vector) {
	return XMFLOAT3(
	vector.x * scaler,
	vector.y * scaler,
	vector.z * scaler
	);
}

const DirectX::XMFLOAT3 operator*(const XMFLOAT3& vector, float scaler) {
	return scaler * vector;
}

//---------------------------------
//   �x�N�g���ƃX�J���[�̏��Z
//---------------------------------
const XMFLOAT3 operator/(float scaler, const XMFLOAT3& vector) {
	assert(scaler == 0 || !("0���Z�_���BMath::DivScaler"));
	return XMFLOAT3(
	vector.x / scaler,
	vector.y / scaler,
	vector.z / scaler
	);
}

const DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& vector, float scaler) {
	return scaler / vector;
}

//----------------------------------------
//  3�����x�N�g����\��
//----------------------------------------
void Vector::Print(const XMFLOAT3& vector) {
	std::cout << " x = " << vector.x << " y = " << vector.y << " z = " << vector.z << "\n";
}


//----------------------------------------
//  �x�N�g���̒����̎擾
//----------------------------------------
float Vector::GetLenght(const XMFLOAT3& vector) {
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

//---------------------------------------
//  �x�N�g���̒����̓����擾
//---------------------------------------
float Vector::GetLenghtSquare(const XMFLOAT3& vector) {
	return (vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}


//--------------------------------------
//   �x�N�g���̐��K��
//--------------------------------------
XMFLOAT3 Vector::GetNormalize(const XMFLOAT3& vector) {
	float lenght = Vector::GetLenght(vector);
	XMFLOAT3 returnVec;
	returnVec.x = vector.x / lenght;
	returnVec.y = vector.y / lenght;
	returnVec.z = vector.z / lenght;
	return returnVec;
}

//-----------------------------------
//  �x�N�g���̓��ς̎擾
//-----------------------------------
constexpr float Vector::GetDot(const XMFLOAT3& vector1, const XMFLOAT3& vector2) {
	return (vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z);
}

//------------------------------------
//  ���g����ڕW���ւ̊p�x�����߂�
//------------------------------------
DirectX::XMFLOAT3 Vector::GetAngleToTarget(const DirectX::XMFLOAT3& ownVector, const DirectX::XMFLOAT3& targetVector) {
	DirectX::XMFLOAT3 ownToTargetVec = targetVector - ownVector;
	DirectX::XMFLOAT3 xVec(1.0f, 0.0f, 0.0f);
	ownToTargetVec.y = 0.0f;
	DirectX::XMFLOAT3 normalizeOwnToTargetVec = Vector::GetNormalize(ownToTargetVec);
	float ownToTargetRot = std::acos(Vector::GetDot(normalizeOwnToTargetVec, xVec));
	ownToTargetRot = DirectX::XMConvertToDegrees(ownToTargetRot);
	if (ownVector.z < targetVector.z) ownToTargetRot *= -1.0f;
	return DirectX::XMFLOAT3(0.0f, ownToTargetRot, 0.0f);
}

//--------------------------------------
//  XMVECTOR�^��XMFLOAT�^�ɕϊ�
//--------------------------------------
DirectX::XMFLOAT3 Vector::ConvertVectorToFloat3(const XMVECTOR& vector) {
	return XMFLOAT3(vector.m128_f32[0], vector.m128_f32[1], vector.m128_f32[2]);
}

DirectX::XMVECTOR Vector::ConvertFloat3ToVector(const XMFLOAT3& float3) {
	return XMVectorSet(float3.x,float3.y,float3.z,1.0f);
}

