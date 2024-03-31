#include "PlayerBehavior.h"
#include "GameObject/GameObject.h"
#include "RigidBody.h"
#include "Common/VectorOperation.h"
#include "AudioComponent.h"
#include "Common/AudioSystem.h"
#include "KeyCounter.h"
#include "Input/Input.h"
#include <iostream>

PlayerBehavior::PlayerBehavior(GameObject& owner,DynamicRigidPtr physics,AudioCompPtrS audio) 
	:mOwner(owner)
	,mPhysics(physics)
	,mAudio(audio)
	,mIsClear(false)
	,mDeadSound(nullptr)
	,mCanBigJump(false)
	,mNextPos()
{
	// 足に取り付けるトリガーの衝突時に呼び出される関数
	auto triggeFunc = [this](const HitObjectInfo& info) {
		const bool isEnter = info.triggerEvent == TriggerEvent::EEnter;
		// 足とカエルが当たったら大ジャンプする
		if (isEnter && info.name == "Kaeru" && (!mPhysics->IsLanding())) {
			mCanBigJump = true; // トリガー関数内で直接addForce()を呼べないので、Update()関数で呼ぶ
		}
	};
	mPhysics->SetTriggerFunc(triggeFunc,"Foot");
	mPhysics->AttachPreventingPenetrationBox(0.6f, 0.5f, 0.6f, {});
}


void PlayerBehavior::Update() {
	mOwner.AddPos(mNextPos);
	// 大ジャンプ出来るなら速度をリセットしてから上方向に力を加える。
	if (mCanBigJump) {
		mPhysics->ResetVelocity();
		mPhysics->AddForce(DirectX::XMFLOAT3(0.0f, 15.5f, 0.0f));
		mCanBigJump = false;
	}
}

bool PlayerBehavior::IsClear() {
	return mIsClear;
}