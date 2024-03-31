#include "GamepadButtonCommand.h"
#include "Input.h"
#include <Xinput.h>

GamepadButtonCommand::GamepadButtonCommand(std::function<void()> action,std::vector<GamepadButtonInput> inputs) 
	:mAction(action)
	,mInputs(inputs)
{

}

GamepadButtonCommand::GamepadButtonCommand(std::function<void()> action,GamepadButtonInput input)
	:mAction(action)
{
	mInputs.push_back(input);
}


void GamepadButtonCommand::Execute()const {
	// 必要な入力がされたら、その入力に対する処理を実行する
	for (auto input : mInputs) {
		if (Input::GetButtonState(input.buttonType) == input.buttonState) { // 必要な入力がされたら
			mAction(); // それに対するアクション
		}
	}
}