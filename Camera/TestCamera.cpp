#include "TestCamera.h"
#include "Common/VectorOperation.h"
#include "Input/Input.h"
#include "Input/MouseMove.h"
#include "Common/QuaternionOperation.h"
#include <xInput.h>
#include <iostream>

TestCamera::TestCamera()
	:mRoll(0.0f)
	,mQuaternion(DirectX::XMQuaternionIdentity())
{
	
}

//-------------------------------------------
//  �J�����̍X�V
//-------------------------------------------
void TestCamera::Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos) {
	Move(cameraPos);
	Rotate();
	UpdateLookPos(cameraPos,lookPos);
}


//---------------------------------------
//  �J�����̒����ʒu���X�V���܂�
//---------------------------------------
void TestCamera::UpdateLookPos(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos) {
	DirectX::XMVECTOR zAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	const DirectX::XMVECTOR frontVector = DirectX::XMVector3Rotate(zAxis,mQuaternion);
	const DirectX::XMVECTOR posVector = DirectX::XMLoadFloat3(&cameraPos);
	DirectX::XMFLOAT3 currentLookPos(posVector.m128_f32[0] + frontVector.m128_f32[0],
					 posVector.m128_f32[1] + frontVector.m128_f32[1],
					 posVector.m128_f32[2] + frontVector.m128_f32[2]);
	lookPos = currentLookPos;
}

//-------------------------------------
//  �J�������ړ�
//-------------------------------------
void TestCamera::Move(DirectX::XMFLOAT3& cameraPos) {

	DirectX::XMFLOAT3 moveVec(0.0f,0.0f,0.0f);

	// �O���ړ�
	if (Input::GetKeyState('A') == InputState::EHold) {
		moveVec = Quaternion::GetLeft(mQuaternion);
	}
	// ����ړ�
	if (Input::GetKeyState('D') == InputState::EHold) {
		moveVec = Quaternion::GetRight(mQuaternion);
	}
	// ���ړ�
	if (Input::GetKeyState('S') == InputState::EHold) {
		moveVec = Quaternion::GetBack(mQuaternion);
	}
	// �E�ړ�
	if (Input::GetKeyState('W') == InputState::EHold) {
		moveVec = Quaternion::GetFront(mQuaternion);
	}
	
	
	DirectX::XMFLOAT2 axis = Input::GetStickAxis();
	moveVec = moveVec + (axis.x * Quaternion::GetBack(mQuaternion));
	moveVec = moveVec + (axis.y * Quaternion::GetRight(mQuaternion));

	

	moveVec.y = 0.0f;  // �㉺�ړ��͖���
	Vector::GetNormalize(moveVec);
	cameraPos = cameraPos + (0.3f * moveVec);

	// ���ړ�
	if (Input::GetKeyState('Z') == InputState::EHold) {
		cameraPos.y = cameraPos.y - 0.1f;
	}
	// ��ړ�
	if (Input::GetKeyState('X') == InputState::EHold) {
		cameraPos.y = cameraPos.y + 0.1f;
	}

	
}

//--------------------------------------
//  �J��������]������
//--------------------------------------
void TestCamera::Rotate() {
	Mouse::Update();
	POINT point = Mouse::GetRelativeMovement();
	
	static bool l = true;
	DirectX::XMFLOAT2 fPoint(static_cast<float>(point.x), static_cast<float>(point.y));
	fPoint.x += Input::GetRStickAxis().x* 10;
	fPoint.y += Input::GetRStickAxis().y* -10;
	fPoint.x *= 0.1f;
	fPoint.y *= 0.05f;
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
	mQuaternion = Quaternion::RotateAxisAndMultiply(DirectX::XMVectorSet(localXAxis.x,localXAxis.y,localXAxis.z,1.0f),fPoint.y, mQuaternion); // ���[�J��x����]
	mQuaternion = Quaternion::RotateEulerAndMultiply(0.0f, fPoint.x, 0.0f, mQuaternion); // y����]�̓��[���h��]�Ŗ��Ȃ�
}