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
//  カメラの更新
//-------------------------------------------
void TestCamera::Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos) {
	Move(cameraPos);
	Rotate();
	UpdateLookPos(cameraPos,lookPos);
}


//---------------------------------------
//  カメラの注視位置を更新します
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
//  カメラを移動
//-------------------------------------
void TestCamera::Move(DirectX::XMFLOAT3& cameraPos) {

	DirectX::XMFLOAT3 moveVec(0.0f,0.0f,0.0f);

	// 前方移動
	if (Input::GetKeyState('A') == InputState::EHold) {
		moveVec = Quaternion::GetLeft(mQuaternion);
	}
	// 後方移動
	if (Input::GetKeyState('D') == InputState::EHold) {
		moveVec = Quaternion::GetRight(mQuaternion);
	}
	// 左移動
	if (Input::GetKeyState('S') == InputState::EHold) {
		moveVec = Quaternion::GetBack(mQuaternion);
	}
	// 右移動
	if (Input::GetKeyState('W') == InputState::EHold) {
		moveVec = Quaternion::GetFront(mQuaternion);
	}
	
	
	DirectX::XMFLOAT2 axis = Input::GetStickAxis();
	moveVec = moveVec + (axis.x * Quaternion::GetBack(mQuaternion));
	moveVec = moveVec + (axis.y * Quaternion::GetRight(mQuaternion));

	

	moveVec.y = 0.0f;  // 上下移動は無し
	Vector::GetNormalize(moveVec);
	cameraPos = cameraPos + (0.3f * moveVec);

	// 下移動
	if (Input::GetKeyState('Z') == InputState::EHold) {
		cameraPos.y = cameraPos.y - 0.1f;
	}
	// 上移動
	if (Input::GetKeyState('X') == InputState::EHold) {
		cameraPos.y = cameraPos.y + 0.1f;
	}

	
}

//--------------------------------------
//  カメラを回転させる
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
	// ロール回転量を制限する
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
	// 回転量を更新
	DirectX::XMFLOAT3 localXAxis = Quaternion::GetRight(mQuaternion);
	mQuaternion = Quaternion::RotateAxisAndMultiply(DirectX::XMVectorSet(localXAxis.x,localXAxis.y,localXAxis.z,1.0f),fPoint.y, mQuaternion); // ローカルx軸回転
	mQuaternion = Quaternion::RotateEulerAndMultiply(0.0f, fPoint.x, 0.0f, mQuaternion); // y軸回転はワールド回転で問題なし
}