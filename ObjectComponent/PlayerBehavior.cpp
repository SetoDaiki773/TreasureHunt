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
	// ���Ɏ��t����g���K�[�̏Փˎ��ɌĂяo�����֐�
	auto triggeFunc = [this](const HitObjectInfo& info) {
		const bool isEnter = info.triggerEvent == TriggerEvent::EEnter;
		// ���ƃJ�G���������������W�����v����
		if (isEnter && info.name == "Kaeru" && (!mPhysics->IsLanding())) {
			mCanBigJump = true; // �g���K�[�֐����Œ���addForce()���ĂׂȂ��̂ŁAUpdate()�֐��ŌĂ�
		}
	};
	mPhysics->SetTriggerFunc(triggeFunc,"Foot");
	mPhysics->AttachPreventingPenetrationBox(0.6f, 0.5f, 0.6f, {});
}


void PlayerBehavior::Update() {
	mOwner.AddPos(mNextPos);
	// ��W�����v�o����Ȃ瑬�x�����Z�b�g���Ă��������ɗ͂�������B
	if (mCanBigJump) {
		mPhysics->ResetVelocity();
		mPhysics->AddForce(DirectX::XMFLOAT3(0.0f, 15.5f, 0.0f));
		mCanBigJump = false;
	}
}

bool PlayerBehavior::IsClear() {
	return mIsClear;
}