#pragma once
#include <functional>
#include <windows.h>
#include <memory>

struct KeyBoardInput {
	int keyType;
	enum class InputState keyState;
};

//=======================================================================================================================
/// [機能] キーボードの入力に対するアクションを管理する。Excute()を呼ぶと、入力があるかどうかを確かめて、実行する
//=======================================================================================================================
class KeyBoardCommand{
public:
	KeyBoardCommand(std::function<void()>,std::vector<KeyBoardInput>&& keyInputs);
	KeyBoardCommand(std::function<void()>,KeyBoardInput keyInput);
	~KeyBoardCommand() {
		mKeyInputs.clear();
	}
	//-----------------------------------------------------------------------------------------
	/// [機能] 入力状態を調べ、入力がされていたら、その入力に対するアクションを実行する
	//-----------------------------------------------------------------------------------------
	void Execute()const;

private:
	std::vector<KeyBoardInput> mKeyInputs;
	std::function<void()> mAction;

};

using KeyBoardInputs = std::vector<KeyBoardInput>;