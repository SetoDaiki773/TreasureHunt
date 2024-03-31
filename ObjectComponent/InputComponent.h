#pragma once
#include "Component.h"
#include <vector>
#include <functional>
#include <memory>
#include <variant>

using Command = std::variant<class GamepadButtonCommand,class GamepadStickCommand,class KeyBoardCommand>;
using Commands = std::vector<Command>;

//==========================================================================
/// [機能] Commandを管理して、必要であれば実行する。
///        Commandとは入力とその入力に対するアクションを定義してモノ
///        例えば、[AボタンまたはBボタンを押したらジャンプする]というのがコマンド
///        このクラスはそれらコマンドの実行関数を呼び出す。
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
