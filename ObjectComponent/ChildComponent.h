#pragma once
#include "ObjectComponent/Component.h"
#include <memory>

//========================================================================
/// [�@�\] �R���X�g���N�^�̑������͑�������e�Ƃ���A�q���ɂȂ�B
///        �q���͐e�Ɠ����������ړ�����B
//========================================================================
class ChildComponent : public Component {
public:
	            // �q�����e�������Ă��邾���ŁA�I�u�W�F�N�g�����݂���K�v�͂Ȃ��̂ŁAweak_ptr�ɂ���
	ChildComponent(class GameObject& owner,std::weak_ptr<class GameObject> parent);

	void Update()override;


private:
	class GameObject& mOwner;
	std::weak_ptr<class GameObject> mParent;

};