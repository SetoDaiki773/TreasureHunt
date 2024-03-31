#pragma once
#include "Component.h"

//=========================================================================================================
/// [機能] プレイヤーの入力によるアクションを定義して、Update()で実行する。(実際に実行するのはこのクラスではない)
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
	std::shared_ptr<class InputComponent> mInputComponent;             // 有効無効のある入力要素
	std::shared_ptr<class InputComponent> mAlwaysValidInputComponent;  // 常に有効な入力要素
};