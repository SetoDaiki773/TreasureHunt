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
	/// [引数]　入力に対するアクション
	/// [引数2] アクションを起こす入力
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
