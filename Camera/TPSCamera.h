#pragma once
#include "CameraMovement.h"
#include <memory>

//==================================================================================
/// [機能] ドラクエ11とかと同じようなカメラ挙動を実行するクラス。targetObjectが注視点
//==================================================================================
class TPSCamera : public CameraMovement{
public:
	explicit TPSCamera(class GameObject* targetObject);
	TPSCamera(class GameObject* targetObject,float pitch,float yaw);
	void Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos)override;

	//------------------------------------------------------
	/// [補足] staticなのはTPSカメラが回転量を受け継ぐから
	//------------------------------------------------------
	static void ResetQuaternion();

private:
	class GameObject* mTargetObject;      // nullの可能性があるのでポインタで受け取る
	static DirectX::XMVECTOR mQuaternion; // カメラの回転量は全てのインスタンスで共通化する
	static float mRoll;                   // 回転を引き継ぐのでロールも当然引き継ぐ
};