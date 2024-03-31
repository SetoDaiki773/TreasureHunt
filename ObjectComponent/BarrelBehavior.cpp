#include "BarrelBehavior.h"
#include "GameObject/GameObject.h"
#include "RigidBody.h"
#include "ChildComponent.h"
#include "Common/VectorOperation.h"
#include "Common/MyMath.h"
#include <DirectXMath.h>

BarrelBehavior::BarrelBehavior(GameObject& owner,GameObjectPtrS person,DynamicRigidPtr rigidBody)
	:mOwner(owner)
	,mPerson(person)
	,mRigidBody(rigidBody)
	,mEnterPersonHead(false)
	,mOutPersonHead(false)
	,mIsOnPersonHead(false)
{
	rigidBody->AttachBoxTrigger(0.99f, 0.3f, 0.99f, {},"BarrelHead",DirectX::XMFLOAT3(0.0f, 0.9f,0.0f));
	rigidBody->AttachBoxTrigger(0.99f, 0.5f, 0.99f, {},"BarrelFoot",DirectX::XMFLOAT3(0.0f,-1.3f,0.0f));
	// �M�̑��̏Փˉ���
	auto makeParent = [this](const HitObjectInfo& info) {
		bool hitPersonHead = (info.name == "PersonHead");
		// �l�ƒM�̑�������������A
		if (hitPersonHead && info.triggerEvent == TriggerEvent::EEnter) {
			mEnterPersonHead = true; // ���ɏ����
		}
		// �l����M�̑������ꂽ��
		if (hitPersonHead && info.triggerEvent == TriggerEvent::EOut) {
			mOutPersonHead = true; // �����痣�ꂽ
		}
	};
	rigidBody->SetTriggerFunc(makeParent, "BarrelFoot");
}

void BarrelBehavior::Update() {
	
	if (mEnterPersonHead) {
		// �e�q�֌W��z���O�ɁA�ʒu��l�̑O�ɂ���B
		DirectX::XMFLOAT3 personPos = mPerson->GetPos();
		personPos.y += 1.96f; // �l�̐^���
		DirectX::XMFLOAT3 barrelNextPos = personPos; // �M�̍��W��
		mOwner.SetPos(barrelNextPos);                // �ʒu�X�V
		mRigidBody->ResetVelocity();
		// �l�Ƃ̐e�q�֌W��z���B�܂�A���ɏ�葱������Ԃ𑱂��悤�ɂ���B
		mOwner.AddSharedComponent(std::make_shared<ChildComponent>(mOwner, mPerson));
		mEnterPersonHead = false;
		mIsOnPersonHead = true;
	}
	if (mOutPersonHead) {
		// �l�Ƃ̐e�q�֌W�𖳂����B�܂�A�l�̓��ɏ�����葱�����Ԃ𖳂���
		mOwner.RemoveComponent<ChildComponent>();
		mOutPersonHead = false;
		mIsOnPersonHead = false;
	}
	
}