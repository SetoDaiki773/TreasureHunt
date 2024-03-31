#pragma once
#include "Component.h"

//=========================================================================================================
/// [�@�\] �v���C���[�̓��͂ɂ��A�N�V�������`���āAUpdate()�Ŏ��s����B(���ۂɎ��s����̂͂��̃N���X�ł͂Ȃ�)
//=========================================================================================================
class PlayerInputComponent : public Component {
public:
	PlayerInputComponent(class GameObject& player,
		                 class Camera* camera,
	                     class AudioComponent* audioComp,
						 class DynamicRigidBody* rigidBody,
						 float jumpPower,
						 bool isActive
	                     );

	void Update()override;
	bool IsActive()const;
	
private:
	class GameObject& mOwner;
	bool mIsActive;
	std::shared_ptr<class InputComponent> mInputComponent;             // �L�������̂�����͗v�f
	std::shared_ptr<class InputComponent> mAlwaysValidInputComponent;  // ��ɗL���ȓ��͗v�f
};