#include "GamepadStickCommand.h"
#include "Input.h"

GamepadStickCommand::GamepadStickCommand(std::function<void(DirectX::XMFLOAT2)> action,StickType stickType) 
	:mAction(action)
	,mStickType(stickType)
{

}


void GamepadStickCommand::Execute()const {
	DirectX::XMFLOAT2 stickAxis = Input::GetStickAxis();
	mAction(stickAxis); // スティック入力によるアクション実行
}