#include "QuaternionOperation.h"
#include "VectorOperation.h"

using namespace DirectX;


//-------------------------------------
//   前方ベクトルの取得
//-------------------------------------
XMFLOAT3 Quaternion::GetFront(const XMVECTOR& quaternion) {
	XMVECTOR xAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR frontVector = XMVector3Rotate(xAxis, quaternion); // +z方向のベクトルをquaternionで回転させれば、前方ベクトル
	return XMFLOAT3(frontVector.m128_f32[0], frontVector.m128_f32[1], frontVector.m128_f32[2]);
}

//-------------------------------------
//   後方ベクトルの取得
//-------------------------------------
XMFLOAT3 Quaternion::GetBack(const XMVECTOR& quaternion) {
	XMFLOAT3 front = Quaternion::GetFront(quaternion); // 前方ベクトルの逆
	return XMFLOAT3(-1 * front);
}

//-------------------------------------
//   右ベクトルの取得
//-------------------------------------
XMFLOAT3 Quaternion::GetRight(const DirectX::XMVECTOR& quaternion) {
	XMVECTOR zAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rightVector = XMVector3Rotate(zAxis, quaternion); // +x方向のベクトルをquaternionで回転させれば、右ベクトル
	return XMFLOAT3(rightVector.m128_f32[0], rightVector.m128_f32[1], rightVector.m128_f32[2]);
}

//-------------------------------------
//   左ベクトルの取得
//-------------------------------------
XMFLOAT3 Quaternion::GetLeft(const XMVECTOR& quaternion) {
	XMFLOAT3 right = Quaternion::GetRight(quaternion); // 右ベクトルの逆
	return XMFLOAT3(-1 * right);
}


//--------------------------------------------
//  回転四元数を度数法で取得
//--------------------------------------------
XMVECTOR Quaternion::RotateEuler(float pitch, float yaw, float roll) {
	// DirectXの数学関数は弧度法を使っているので、度数法から弧度法に変換
	pitch = XMConvertToRadians(pitch);
	yaw = XMConvertToRadians(yaw);
	roll = XMConvertToRadians(roll);
	// オイラー回転関数を呼ぶ
	return XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
}


XMVECTOR Quaternion::RotateAxis(const DirectX::XMVECTOR& axis, float angleDegree) {
	float angleRadian = XMConvertToRadians(angleDegree);
	return XMQuaternionRotationAxis(axis, angleDegree);
}


//--------------------------------------------
//  クォータニオンの回転と乗算
//--------------------------------------------
XMVECTOR Quaternion::RotateEulerAndMultiply(float pitch, float yaw, float roll, const XMVECTOR& srcQuaternion) {
	XMVECTOR quaternion = RotateEuler(pitch, yaw, roll);
	XMVECTOR destQuaternion = XMQuaternionMultiply(srcQuaternion, quaternion);
	return destQuaternion;
}

XMVECTOR Quaternion::RotateAxisAndMultiply(const XMVECTOR& axis, float degreeAngle, const XMVECTOR& srcQuaternion) {
	XMVECTOR quaternion = RotateAxis(axis, degreeAngle);
	XMVECTOR destQuaternion = XMQuaternionMultiply(srcQuaternion, quaternion);
	return destQuaternion;
}

