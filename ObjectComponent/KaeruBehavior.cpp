#include "KaeruBehavior.h"
#include "RigidBody.h"
#include "GameObject/GameObject.h"
#include "PlayerInputComponent.h"
#include "ChildComponent.h"
#include "AudioComponent.h"
#include "Common/AudioSystem.h"
#include "KeyCounter.h"
#include "Common/VectorOperation.h"
#include <iostream>


KaeruBehavior::KaeruBehavior(GameObject& owner, DynamicRigidPtr physics,
	                         GameObjectPtrS person, std::shared_ptr<PlayerInputComponent> input,
	                         AudioCompPtrS audio) 
	:mOwner(owner)
	,mPhysics(physics)
	,mPerson(person)
	,mInput(input)
	,mAudio(audio)
	,mEnterPersonHead(false)
	,mOutPersonHead(false)
{
	physics->AttachPreventingPenetrationBox(0.7f,0.3f,0.7f); // ブロック貫通対策の当たり判定をつける
	// カエルの足の衝突応答
	auto makeParent = [this](const HitObjectInfo& info) {
		// 人とカエルの足が当たったら、
		if (info.name == "PersonHead" && info.triggerEvent == TriggerEvent::EEnter) {
			mEnterPersonHead = true;
		}
		// 人とカエルの足が離れたら
		if (info.name == "PersonHead" && info.triggerEvent == TriggerEvent::EOut) {
			mOutPersonHead = true;
		}
	};
	physics->SetTriggerFunc(makeParent,"KaeruFoot");
}


void KaeruBehavior::Update() {
	if (mEnterPersonHead) {
		// 人との親子関係を築く。つまり、頭に乗り続けた状態を続くようにする。
		mOwner.AddSharedComponent(std::make_shared<ChildComponent>(mOwner, mPerson));
		mEnterPersonHead = false;
	}
	if (mOutPersonHead) {
		// 人との親子関係を無くす。つまり、人の頭に乗っかり続ける状態を無くす
		mOwner.RemoveComponent<ChildComponent>();
		mOutPersonHead = false;
	}
	// 位置が-10を下回ったらゲームオーバー
	if (mOwner.GetPos().y <= -10.0f) {
		mAudio->PlaySoundEvent("event:/KaeruDead");
		mOwner.SetState(GameObject::State::EStopUndraw); // 状態をストップ勝つ描画しないようにする。Deadにするとサウンドが停止してしまう。
	}
}