#pragma once
#include <DirectXMath.h>
#include <memory>

class ObjectResource {
public:
	virtual void Render(const DirectX::XMMATRIX&, const class Camera&) = 0;
};

using ObjectResourcePtrS = std::shared_ptr<ObjectResource>;
