#include "Camera/TPSCamera.h"
#include "GameObject/GameObject.h"
#include "ObjectComponent/WorldTransform.h"
#include "Input/Input.h"
#include "Input/MouseMove.h"
#include "Common/QuaternionOperation.h"
#include "Common/VectorOperation.h"
#include <algorithm>

DirectX::XMVECTOR TPSCamera::mQuaternion(DirectX::XMQuaternionIdentity());
float TPSCamera::mRoll(0.0f);

TPSCamera::TPSCamera(GameObject* targetObject)
	:mTargetObject(targetObject)
{

}

TPSCamera::TPSCamera(GameObject* targetObject,float pitch,float yaw) 
	:mTargetObject(targetObject)
{
	mQuaternion = Quaternion::RotateEuler(pitch,yaw,mRoll);
}

void TPSCamera::ResetQuaternion() {
	mQuaternion = DirectX::XMQuaternionIdentity();
	mRoll = 0.0f;
}


void TPSCamera::Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos) {
	if (mTargetObject == nullptr) return;
	lookPos = mTargetObject->GetPos(); // �����ʒu�̓^�[�Q�b�g�ʒu
	

	// �}�E�X��R���g���[���[�ŃJ��������]������
	Mouse::Update();
	POINT point = Mouse::GetRelativeMovement(); // �}�E�X�̑O�t���[������̈ړ��ʎ擾

	DirectX::XMFLOAT2 fPoint(static_cast<float>(point.x), static_cast<float>(point.y)); // �}�E�X�̈ړ��ʂ����Ƃ�XMFLOAT2������
	fPoint.x += Input::GetRStickAxis().x * 10;   // �Q�[���p�b�h�̃X�e�B�b�N���͒l�����f����
	fPoint.y += Input::GetRStickAxis().y * -10;
	fPoint.x *= 0.1f;  // �傫������
	fPoint.y *= 0.05f; // �傫������
	mRoll += fPoint.y;
	// ���[����]�ʂ𐧌�����
	if (mRoll >= 10.0f)
	{
		fPoint.y = 0.0f;
		mRoll = 10.0f;
	}
	if (mRoll <= -10.0f) {
		fPoint.y = 0.0f;
		mRoll = -10.0f;
	}
	fPoint.y *= 0.05f;
	// ��]�ʂ��X�V
	DirectX::XMFLOAT3 localXAxis = Quaternion::GetRight(mQuaternion);
	// �E�x�N�g�������Ƀ}�E�X��X�e�B�b�N��y�ړ��ʂŉ�]������B��������̃N�H�[�^�j�I���ɏ�Z�B�J������X����]�ʂ����܂�B
	mQuaternion = Quaternion::RotateAxisAndMultiply(DirectX::XMVectorSet(localXAxis.x, localXAxis.y, localXAxis.z, 1.0f), fPoint.y, mQuaternion); // ���[�J��x����]
	// ���[���h���W�n��y�������x�ړ��ʂ��g���ĉ�]
	mQuaternion = Quaternion::RotateEulerAndMultiply(0.0f, fPoint.x, 0.0f, mQuaternion); // y����]�̓��[���h��]�Ŗ��Ȃ�
	// �J�����̉�]������x�N�g���𓾂āA�����瑤�Ɉړ�����
	cameraPos = lookPos;
	DirectX::XMFLOAT3 backVec = Quaternion::GetBack(mQuaternion);
	cameraPos = cameraPos + (20.0f * backVec);
	cameraPos.y += 10.0f;
	
}