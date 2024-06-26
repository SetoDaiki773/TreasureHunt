#pragma once

//--------------------------------
/// 入力に対する処理を実行する
//--------------------------------
struct CommandExecuter {
	void operator()(const class GamepadButtonCommand& command);
	void operator()(const class GamepadStickCommand& command);
	void operator()(const class KeyBoardCommand& command);

};
