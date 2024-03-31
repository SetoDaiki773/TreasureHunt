#include "GamepadStickCommand.h"
#include "Input.h"

GamepadStickCommand::GamepadStickCommand(std::function<void(DirectX::XMFLOAT2)> action,StickType stickType) 
	:mAction(action)
	,mStickType(stickType)
{

}


void GamepadStickCommand::Execute()const {
	DirectX::XMFLOAT2 stickAxis = Input::GetStickAxis();
	mAction(stickAxis); // �X�e�B�b�N���͂ɂ��A�N�V�������s
}