#include "InputComponent.h"
#include "GameObject/GameObject.h"
#include "Input/GamepadButtonCommand.h"
#include "Input/GamepadStickCommand.h"
#include "Input/KeyBoardCommand.h"
#include "Input/CommandExecuter.h"

struct InputCompPimpl {
	~InputCompPimpl() {
		commands.clear();
	}
	Commands commands;
};


InputComponent::InputComponent(GameObject& owner)
	:mOwner(owner)
	,mCommands(std::make_unique<InputCompPimpl>())
{

}

InputComponent::InputComponent(GameObject& owner,Commands commands)
	:mOwner(owner)
	,mCommands(std::make_unique<InputCompPimpl>(std::move(commands)))
{

}

InputComponent::InputComponent(const InputComponent& inputComp)
	:mOwner(inputComp.mOwner)
	,mCommands(std::make_unique<InputCompPimpl>(*inputComp.mCommands))
{
	
}

InputComponent& InputComponent::operator=(const InputComponent& inputComp) {
	*mCommands = *inputComp.mCommands;
	return *this;
}

InputComponent::~InputComponent() = default;
InputComponent::InputComponent(InputComponent&&) = default;



void InputComponent::Update() {
	// 全てのコマンドを実行
	for (auto& c : mCommands->commands) {
		std::visit(CommandExecuter{},c);
	}
}


void InputComponent::AddCommand(Command&& command) {
	mCommands->commands.push_back(std::move(command));
}