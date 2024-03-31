#pragma once
#include "Input/GamepadButtonCommand.h"
#include "Input/GamepadStickCommand.h"
#include "Input/KeyBoardCommand.h"
#include <variant>
#include <vector>

using Command = std::variant<GamepadButtonCommand, GamepadStickCommand, KeyBoardCommand>;
using Commands = std::vector<Command>;

class InputComponentPimpl {
public:


private:
	Commands mCommands;
};