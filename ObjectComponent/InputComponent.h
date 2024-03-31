#pragma once
#include "Component.h"
#include <vector>
#include <functional>
#include <memory>
#include <variant>

using Command = std::variant<class GamepadButtonCommand,class GamepadStickCommand,class KeyBoardCommand>;
using Commands = std::vector<Command>;

//==========================================================================
/// [�@�\] Command���Ǘ����āA�K�v�ł���Ύ��s����B
///        Command�Ƃ͓��͂Ƃ��̓��͂ɑ΂���A�N�V�������`���ă��m
///        �Ⴆ�΁A[A�{�^���܂���B�{�^������������W�����v����]�Ƃ����̂��R�}���h
///        ���̃N���X�͂����R�}���h�̎��s�֐����Ăяo���B
//==========================================================================
class InputComponent : public Component{
public:
	explicit InputComponent(class GameObject& mOwner);
	InputComponent(class GameObject& mOwner,Commands commands);
	~InputComponent();
	InputComponent(const InputComponent& inputComponent);
	InputComponent(InputComponent&& inputComponent);
	InputComponent& operator=(const InputComponent& inputComponent);
	
	void Update()override;
	void AddCommand(Command&& command);

private:
	class GameObject& mOwner;
	std::unique_ptr<struct InputCompPimpl> mCommands;

};
