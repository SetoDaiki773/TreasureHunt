#pragma once
#include "Component.h"
#include <DirectXMath.h>

class BarrelBehavior : public Component {
public:
	BarrelBehavior(class GameObject& owner,std::shared_ptr<class GameObject> person,std::shared_ptr<class DynamicRigidBody> rigidBody);

	void Update()override;

private:
	class GameObject& mOwner;
	std::shared_ptr<class GameObject> mPerson;
	std::shared_ptr<class DynamicRigidBody> mRigidBody;
	bool mEnterPersonHead;
	bool mOutPersonHead;
	bool mIsOnPersonHead;
};
