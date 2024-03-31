#pragma once
#include <functional>
#include <memory>


struct GamepadButtonInput {
	int buttonType;
	enum class InputState buttonState;
};

class GamepadButtonCommand{
public:
	//-----------------------------------
	/// [����]�@���͂ɑ΂���A�N�V����
	/// [����2] �A�N�V�������N��������
	//-----------------------------------
	GamepadButtonCommand(std::function<void()> actionOnInputs, std::vector<GamepadButtonInput> inputs);
	GamepadButtonCommand(std::function<void()> actionOnInput, GamepadButtonInput input);
	~GamepadButtonCommand() {
		mInputs.clear();
	}
	void Execute()const;

private:
	std::function<void()> mAction;
	std::vector<GamepadButtonInput> mInputs;
};
