#pragma once
#include "CameraMovement.h"
#include <memory>

class TestCamera :public CameraMovement {
public:
	TestCamera();
	
	//-------------------------------------
	/// [機能]カメラの状態の更新
	//-------------------------------------
	void Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos)override;
	

private:
	void Move(DirectX::XMFLOAT3& cameraPos);
	void Rotate();
	void UpdateLookPos(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos);

	DirectX::XMVECTOR mQuaternion;
	float mRoll;
};
