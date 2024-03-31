#pragma once
#include <memory>
#include "Component.h"

//================================================
/// [機能] カエル固有の動きを更新する
//================================================
class KaeruBehavior : public Component {
public:
	KaeruBehavior(class GameObject& owner,
		          std::shared_ptr<class DynamicRigidBody> physics,
		          std::shared_ptr<class GameObject> person,
		          std::shared_ptr<class PlayerInputComponent> inputComp,
		          std::shared_ptr<class AudioComponent> audio
	             );
	void Update()override;

private:
	class GameObject& mOwner;
	std::shared_ptr<class DynamicRigidBody> mPhysics;
	std::shared_ptr<class GameObject> mPerson;
	std::shared_ptr<class PlayerInputComponent> mInput;
	std::shared_ptr<class AudioComponent> mAudio;
	bool mEnterPersonHead;
	bool mOutPersonHead;
};
