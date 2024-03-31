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
	// 樽の足の衝突応答
	auto makeParent = [this](const HitObjectInfo& info) {
		bool hitPersonHead = (info.name == "PersonHead");
		// 人と樽の足が当たったら、
		if (hitPersonHead && info.triggerEvent == TriggerEvent::EEnter) {
			mEnterPersonHead = true; // 頭に乗った
		}
		// 人から樽の足が離れたら
		if (hitPersonHead && info.triggerEvent == TriggerEvent::EOut) {
			mOutPersonHead = true; // 頭から離れた
		}
	};
	rigidBody->SetTriggerFunc(makeParent, "BarrelFoot");
}

void BarrelBehavior::Update() {
	
	if (mEnterPersonHead) {
		// 親子関係を築く前に、位置を人の前にする。
		DirectX::XMFLOAT3 personPos = mPerson->GetPos();
		personPos.y += 1.96f; // 人の真上を
		DirectX::XMFLOAT3 barrelNextPos = personPos; // 樽の座標に
		mOwner.SetPos(barrelNextPos);                // 位置更新
		mRigidBody->ResetVelocity();
		// 人との親子関係を築く。つまり、頭に乗り続けた状態を続くようにする。
		mOwner.AddSharedComponent(std::make_shared<ChildComponent>(mOwner, mPerson));
		mEnterPersonHead = false;
		mIsOnPersonHead = true;
	}
	if (mOutPersonHead) {
		// 人との親子関係を無くす。つまり、人の頭に乗っかり続ける状態を無くす
		mOwner.RemoveComponent<ChildComponent>();
		mOutPersonHead = false;
		mIsOnPersonHead = false;
	}
	
}