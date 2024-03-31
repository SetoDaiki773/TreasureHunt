#include "ChildComponent.h"
#include "GameObject/GameObject.h"
#include "Common/VectorOperation.h"

ChildComponent::ChildComponent(GameObject& owner,std::weak_ptr<GameObject> parent) 
	:mOwner(owner)
	,mParent(parent)
{

}


void ChildComponent::Update() {
	if (mParent.expired()) return;  // �Q�Ɛ悪�Ȃ��Ȃ�A���������߂�
	// �e�́A�O�t���[�����猻�݃t���[���܂ł̈ړ���
	DirectX::XMFLOAT3 parentMovementAmount = mParent.lock()->GetPos() - mParent.lock()->GetPrevFramePos(); 
	mOwner.AddPos(parentMovementAmount);  // ��t���[���œ������������A�q���������B
}