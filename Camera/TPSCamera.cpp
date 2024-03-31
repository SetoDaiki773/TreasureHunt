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
	lookPos = mTargetObject->GetPos(); // 注視位置はターゲット位置
	

	// マウスやコントローラーでカメラを回転させる
	Mouse::Update();
	POINT point = Mouse::GetRelativeMovement(); // マウスの前フレームからの移動量取得

	DirectX::XMFLOAT2 fPoint(static_cast<float>(point.x), static_cast<float>(point.y)); // マウスの移動量をもとにXMFLOAT2をつくる
	fPoint.x += Input::GetRStickAxis().x * 10;   // ゲームパッドのスティック入力値も反映する
	fPoint.y += Input::GetRStickAxis().y * -10;
	fPoint.x *= 0.1f;  // 大きさ調整
	fPoint.y *= 0.05f; // 大きさ調整
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
	// 右ベクトルを軸にマウスやスティックのy移動量で回転させる。それを元のクォータニオンに乗算。カメラのX軸回転量が求まる。
	mQuaternion = Quaternion::RotateAxisAndMultiply(DirectX::XMVectorSet(localXAxis.x, localXAxis.y, localXAxis.z, 1.0f), fPoint.y, mQuaternion); // ローカルx軸回転
	// ワールド座標系のy軸周りにx移動量を使って回転
	mQuaternion = Quaternion::RotateEulerAndMultiply(0.0f, fPoint.x, 0.0f, mQuaternion); // y軸回転はワールド回転で問題なし
	// カメラの回転から後ろベクトルを得て、そちら側に移動する
	cameraPos = lookPos;
	DirectX::XMFLOAT3 backVec = Quaternion::GetBack(mQuaternion);
	cameraPos = cameraPos + (20.0f * backVec);
	cameraPos.y += 10.0f;
	
}