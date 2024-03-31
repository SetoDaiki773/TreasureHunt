#include "CommandExecuter.h"
#include "GamepadButtonCommand.h"
#include "GamepadStickCommand.h"
#include "KeyBoardCommand.h"
//--------------------------------
// 型に応じたコマンドを実行
//--------------------------------

void CommandExecuter::operator()(const GamepadButtonCommand& command) {
	command.Execute();
}

void CommandExecuter::operator()(const GamepadStickCommand& command) {
	command.Execute();
}

void CommandExecuter::operator()(const KeyBoardCommand& command) {
	command.Execute();
}

