#pragma once
#include <windows.h>
#include <XInput.h>
#include <DirectXMath.h>

#pragma comment(lib,"xinput.lib")

//===============================================
//  keyの状態列挙型
//===============================================
enum class InputState {
	EPush,
	EHold,
	ERelease,
	ENone
};


class Input {
public:
	

	//-------------------------------------------------------------------
	/// [機能] 最新のkeyのStateの取得する
	/// [引数] XInputに定義されたkeyの名前(実態がint型であるだけ)
	//-------------------------------------------------------------------
	static InputState GetKeyState(int keyCode);

	//-------------------------------------------------------------------
	/// [機能] 最新のkeyのStateの取得
	/// [引数] XInputに定義されたkeyの名前(実態がint型であるだけ)
	//-------------------------------------------------------------------
	static InputState GetButtonState(int button);
	//-----------------------------------------------
	/// [機能] スティックの傾きを返す。
	//-----------------------------------------------
	static DirectX::XMFLOAT2 GetStickAxis();
	static DirectX::XMFLOAT2 GetRStickAxis();

	//-------------------------------------------------------------------------------
	/// [機能] 最新の押されたkeyのkeyStateと最新の押されたkyeを更新します
	/// [使用方法/注意点] インスタンスの数関係なく一回のループにつき一度だけ呼び出します
	//-------------------------------------------------------------------------------
	static void UpdateCurrentKey();

private:
	Input();    // シングルトンなのでコンストラクター封印
	~Input();   // シングルトンなのでデストラクター封印
	Input(const Input&) = delete;  // シングルトンなのでコピーコンストラクター封印


	static void UpdateGamePadState();
	static void UpdateKeyboardKeyState();
	static BYTE mPrevKey[256];
	static BYTE mCurrentKey[256];
	static XINPUT_GAMEPAD mPrevGamePad;
	static XINPUT_GAMEPAD mCurrentGamePad;
	static DirectX::XMFLOAT2 mRStickAxis;
	static DirectX::XMFLOAT2 mLStickAxis;
};

