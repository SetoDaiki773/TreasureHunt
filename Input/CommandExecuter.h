#pragma once

//--------------------------------
/// “ü—Í‚É‘Î‚·‚éˆ—‚ğÀs‚·‚é
//--------------------------------
struct CommandExecuter {
	void operator()(const class GamepadButtonCommand& command);
	void operator()(const class GamepadStickCommand& command);
	void operator()(const class KeyBoardCommand& command);

};
