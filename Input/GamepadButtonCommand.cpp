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
	// �K�v�ȓ��͂����ꂽ��A���̓��͂ɑ΂��鏈�������s����
	for (auto input : mInputs) {
		if (Input::GetButtonState(input.buttonType) == input.buttonState) { // �K�v�ȓ��͂����ꂽ��
			mAction(); // ����ɑ΂���A�N�V����
		}
	}
}