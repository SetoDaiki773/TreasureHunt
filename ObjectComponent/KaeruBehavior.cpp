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
	physics->AttachPreventingPenetrationBox(0.7f,0.3f,0.7f); // �u���b�N�ђʑ΍�̓����蔻�������
	// �J�G���̑��̏Փˉ���
	auto makeParent = [this](const HitObjectInfo& info) {
		// �l�ƃJ�G���̑�������������A
		if (info.name == "PersonHead" && info.triggerEvent == TriggerEvent::EEnter) {
			mEnterPersonHead = true;
		}
		// �l�ƃJ�G���̑������ꂽ��
		if (info.name == "PersonHead" && info.triggerEvent == TriggerEvent::EOut) {
			mOutPersonHead = true;
		}
	};
	physics->SetTriggerFunc(makeParent,"KaeruFoot");
}


void KaeruBehavior::Update() {
	if (mEnterPersonHead) {
		// �l�Ƃ̐e�q�֌W��z���B�܂�A���ɏ�葱������Ԃ𑱂��悤�ɂ���B
		mOwner.AddSharedComponent(std::make_shared<ChildComponent>(mOwner, mPerson));
		mEnterPersonHead = false;
	}
	if (mOutPersonHead) {
		// �l�Ƃ̐e�q�֌W�𖳂����B�܂�A�l�̓��ɏ�����葱�����Ԃ𖳂���
		mOwner.RemoveComponent<ChildComponent>();
		mOutPersonHead = false;
	}
	// �ʒu��-10�����������Q�[���I�[�o�[
	if (mOwner.GetPos().y <= -10.0f) {
		mAudio->PlaySoundEvent("event:/KaeruDead");
		mOwner.SetState(GameObject::State::EStopUndraw); // ��Ԃ��X�g�b�v���`�悵�Ȃ��悤�ɂ���BDead�ɂ���ƃT�E���h����~���Ă��܂��B
	}
}