#include "QuaternionOperation.h"
#include "VectorOperation.h"

using namespace DirectX;


//-------------------------------------
//   �O���x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Quaternion::GetFront(const XMVECTOR& quaternion) {
	XMVECTOR xAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR frontVector = XMVector3Rotate(xAxis, quaternion); // +z�����̃x�N�g����quaternion�ŉ�]������΁A�O���x�N�g��
	return XMFLOAT3(frontVector.m128_f32[0], frontVector.m128_f32[1], frontVector.m128_f32[2]);
}

//-------------------------------------
//   ����x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Quaternion::GetBack(const XMVECTOR& quaternion) {
	XMFLOAT3 front = Quaternion::GetFront(quaternion); // �O���x�N�g���̋t
	return XMFLOAT3(-1 * front);
}

//-------------------------------------
//   �E�x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Quaternion::GetRight(const DirectX::XMVECTOR& quaternion) {
	XMVECTOR zAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rightVector = XMVector3Rotate(zAxis, quaternion); // +x�����̃x�N�g����quaternion�ŉ�]������΁A�E�x�N�g��
	return XMFLOAT3(rightVector.m128_f32[0], rightVector.m128_f32[1], rightVector.m128_f32[2]);
}

//-------------------------------------
//   ���x�N�g���̎擾
//-------------------------------------
XMFLOAT3 Quaternion::GetLeft(const XMVECTOR& quaternion) {
	XMFLOAT3 right = Quaternion::GetRight(quaternion); // �E�x�N�g���̋t
	return XMFLOAT3(-1 * right);
}


//--------------------------------------------
//  ��]�l������x���@�Ŏ擾
//--------------------------------------------
XMVECTOR Quaternion::RotateEuler(float pitch, float yaw, float roll) {
	// DirectX�̐��w�֐��͌ʓx�@���g���Ă���̂ŁA�x���@����ʓx�@�ɕϊ�
	pitch = XMConvertToRadians(pitch);
	yaw = XMConvertToRadians(yaw);
	roll = XMConvertToRadians(roll);
	// �I�C���[��]�֐����Ă�
	return XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
}


XMVECTOR Quaternion::RotateAxis(const DirectX::XMVECTOR& axis, float angleDegree) {
	float angleRadian = XMConvertToRadians(angleDegree);
	return XMQuaternionRotationAxis(axis, angleDegree);
}


//--------------------------------------------
//  �N�H�[�^�j�I���̉�]�Ə�Z
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

