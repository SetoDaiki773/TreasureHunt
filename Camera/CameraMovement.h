#pragma once
#include <DirectXMath.h>

class CameraMovement {
public:
	virtual ~CameraMovement() {};
	virtual void Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& cameraLookPos) = 0;
};
