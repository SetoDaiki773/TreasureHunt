#include "ChildComponent.h"
#include "GameObject/GameObject.h"
#include "Common/VectorOperation.h"

ChildComponent::ChildComponent(GameObject& owner,std::weak_ptr<GameObject> parent) 
	:mOwner(owner)
	,mParent(parent)
{

}


void ChildComponent::Update() {
	if (mParent.expired()) return;  // 参照先がないなら、何もせず戻る
	// 親の、前フレームから現在フレームまでの移動量
	DirectX::XMFLOAT3 parentMovementAmount = mParent.lock()->GetPos() - mParent.lock()->GetPrevFramePos(); 
	mOwner.AddPos(parentMovementAmount);  // 一フレームで動いた分だけ、子供も動く。
}