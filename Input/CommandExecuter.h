#pragma once

//--------------------------------
/// ���͂ɑ΂��鏈�������s����
//--------------------------------
struct CommandExecuter {
	void operator()(const class GamepadButtonCommand& command);
	void operator()(const class GamepadStickCommand& command);
	void operator()(const class KeyBoardCommand& command);

};
