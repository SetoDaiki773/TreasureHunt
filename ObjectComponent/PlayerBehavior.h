#pragma once
#include "Component.h"
#include <memory>
#include <DirectXmath.h>

//====================================================
/// [機能] プレイヤー固有の動きを更新する。
//====================================================
class PlayerBehavior : public Component{
public:
	PlayerBehavior(class GameObject& owner,
		           std::shared_ptr<class DynamicRigidBody> physics,
		           std::shared_ptr<class AudioComponent> audio
				   );

	void Update()override;
	bool IsClear();

private:
	class GameObject& mOwner;
	std::shared_ptr<class DynamicRigidBody> mPhysics;
	std::shared_ptr<class AudioComponent> mAudio;
	bool mIsClear;
	bool mCanBigJump;
	DirectX::XMFLOAT3 mNextPos;
	std::unique_ptr<class SoundEvent> mDeadSound;
};
