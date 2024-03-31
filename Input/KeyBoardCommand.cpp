#include "KeyBoardCommand.h"
#include "Input.h"


KeyBoardCommand::KeyBoardCommand(std::function<void()> action, std::vector<KeyBoardInput>&& keyInputs)
	:mKeyInputs(std::move(keyInputs))
	,mAction(action)
{

}

KeyBoardCommand::KeyBoardCommand(std::function<void()> action,KeyBoardInput keyInput)
	:mAction(action)
{
	mKeyInputs.push_back(keyInput);
}

//---------------------------------------------
// コマンド実行
//---------------------------------------------
void KeyBoardCommand::Execute()const {
	for (auto key : mKeyInputs) {
		if (Input::GetKeyState(key.keyType) == key.keyState) { // 必要な入力がされたら
			mAction();                                         // アクションを実行
		}
	}
}