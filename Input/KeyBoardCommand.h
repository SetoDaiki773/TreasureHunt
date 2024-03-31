#pragma once
#include <functional>
#include <windows.h>
#include <memory>

struct KeyBoardInput {
	int keyType;
	enum class InputState keyState;
};

//=======================================================================================================================
/// [�@�\] �L�[�{�[�h�̓��͂ɑ΂���A�N�V�������Ǘ�����BExcute()���ĂԂƁA���͂����邩�ǂ������m���߂āA���s����
//=======================================================================================================================
class KeyBoardCommand{
public:
	KeyBoardCommand(std::function<void()>,std::vector<KeyBoardInput>&& keyInputs);
	KeyBoardCommand(std::function<void()>,KeyBoardInput keyInput);
	~KeyBoardCommand() {
		mKeyInputs.clear();
	}
	//-----------------------------------------------------------------------------------------
	/// [�@�\] ���͏�Ԃ𒲂ׁA���͂�����Ă�����A���̓��͂ɑ΂���A�N�V���������s����
	//-----------------------------------------------------------------------------------------
	void Execute()const;

private:
	std::vector<KeyBoardInput> mKeyInputs;
	std::function<void()> mAction;

};

using KeyBoardInputs = std::vector<KeyBoardInput>;