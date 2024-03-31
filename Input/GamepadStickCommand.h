#pragma once
#include <functional>
#include <DirectXMath.h>

enum StickType {
	ERStick,
	ELStick
};

//=============================================================
/// [機能] スティックのタイプに対するアクションを管理する。
//=============================================================
class GamepadStickCommand{
public:
	GamepadStickCommand(std::function<void(DirectX::XMFLOAT2 stickAxis)>,StickType stickType);
	//-----------------------------------------------------------
	/// [機能] スティックの傾きをもとにアクション関数を呼び出す
	//-----------------------------------------------------------
	void Execute()const;

private:
	std::function<void(DirectX::XMFLOAT2)> mAction;
	StickType mStickType;

};
