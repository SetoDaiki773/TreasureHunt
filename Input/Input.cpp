#include "Input.h"
#include <cmath>
#include <iostream>
#include <string.h>
#include <cassert>


BYTE Input::mCurrentKey[256];
BYTE Input::mPrevKey[256];
XINPUT_GAMEPAD Input::mPrevGamePad;
XINPUT_GAMEPAD Input::mCurrentGamePad;
DirectX::XMFLOAT2 Input::mRStickAxis;
DirectX::XMFLOAT2 Input::mLStickAxis;

Input::Input() {
	
}

Input::~Input() {

}

//-------------------------------------------
//  KeyStateの取得
//-------------------------------------------
InputState Input::GetKeyState(int keyCode) {
	// keyStateの更新

	// 前のフレームで押されていたら
	if (mPrevKey[keyCode] & 0x80) {
		// 今回も押されているなら
		if (mCurrentKey[keyCode] & 0x80) {
			return InputState::EHold; // Hold状態を返す
		}
		//今回は押されていないなら
		else {
			return InputState::ERelease; // Release状態を返す
		}
	}

	//前のフレームで押されていないなら
	else {
		//今回は押されているなら
		if (mCurrentKey[keyCode] & 0x80) {
			return InputState::EPush; // Push状態を返す
		}
		//今回も押されていないなら
		else {
			return InputState::ENone; // None状態を返す
		}
	}
}

//------------------------------
//  GamePadボタン状態の取得
//------------------------------
InputState Input::GetButtonState(int button) {
	// 前のフレームで押されていたら
	if (mPrevGamePad.wButtons & button) {
		// 今回も押されているなら
		if (mCurrentGamePad.wButtons & button) {
			return InputState::EHold; // Hold状態を返す
		}
		//今回は押されていないなら
		else {
			return InputState::ERelease; // Release状態を返す
		}
	}

	//前のフレームで押されていないなら
	else {
		//今回は押されているなら
		if (mCurrentGamePad.wButtons & button) {
			return InputState::EPush; // Push状態を返す
		}
		//今回も押されていないなら
		else {
			return InputState::ENone; // None状態を返す
		}
	}
}

//---------------------------
//  スティック座標の取得
//---------------------------
DirectX::XMFLOAT2 Input::GetStickAxis() {
	return mLStickAxis;
}

DirectX::XMFLOAT2 Input::GetRStickAxis() {
	return mRStickAxis;
}

//--------------------------------------------
//　　最新のkeyと最新のkeyStateの更新
//--------------------------------------------
void Input::UpdateCurrentKey() {
	UpdateGamePadState();
	UpdateKeyboardKeyState();
}


//----------------------------------------
//  GamePad状態の更新
//----------------------------------------
void Input::UpdateGamePadState() {
	XINPUT_STATE s;
	ZeroMemory(&s, sizeof(s));
	if (XInputGetState(0, &s) != ERROR_SUCCESS) return;

	// デッドゾーンのコントローラー入力は0に
	if (s.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		s.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		s.Gamepad.sThumbLX = 0;
	}
	if (s.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		s.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		s.Gamepad.sThumbLY = 0;
	}
	mPrevGamePad = mCurrentGamePad;
	mCurrentGamePad = s.Gamepad;

	// 左スティック入力値を-1 ～ 1にする
	float rStickX = static_cast<float>(s.Gamepad.sThumbLX);
	float rStickY = static_cast<float>(s.Gamepad.sThumbLY);
	if (rStickX < 0.0f) {
		rStickX = rStickX / 32768.0f;
	}
	else {
		rStickX = rStickX / 32767.0f;
	}
	if (rStickY < 0.0f) {
		rStickY = rStickY / 32768.0f;
	}
	else {
		rStickY = rStickY / 32767.0f;
	}
	mLStickAxis.x = rStickX;
	mLStickAxis.y = rStickY;

	// 右スティック入力値を-1 ～ 1にする
	float lStickX = static_cast<float>(s.Gamepad.sThumbRX);
	float lStickY = static_cast<float>(s.Gamepad.sThumbRY);
	if (lStickX < 0.0f) {
		lStickX = lStickX / 32768.0f;
	}
	else {
		lStickX = lStickX / 32767.0f;
	}
	if (lStickY < 0.0f) {
		lStickY = lStickY / 32768.0f;
	}
	else {
		lStickY = lStickY / 32767.0f;
	}
	mRStickAxis.x = lStickX;
	mRStickAxis.y = lStickY;
	
}

//---------------------------------------
//  キーボード状態の更新
//---------------------------------------
void Input::UpdateKeyboardKeyState() {
	memcpy(mPrevKey, mCurrentKey, sizeof(BYTE) * 256);
	// 最新のkeyの更新
	if (!GetKeyboardState(mCurrentKey)) {
		assert(!"最新のキーボードステートの取得に失敗");
	}
}

