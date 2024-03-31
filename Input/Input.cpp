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
//  KeyState�̎擾
//-------------------------------------------
InputState Input::GetKeyState(int keyCode) {
	// keyState�̍X�V

	// �O�̃t���[���ŉ�����Ă�����
	if (mPrevKey[keyCode] & 0x80) {
		// �����������Ă���Ȃ�
		if (mCurrentKey[keyCode] & 0x80) {
			return InputState::EHold; // Hold��Ԃ�Ԃ�
		}
		//����͉�����Ă��Ȃ��Ȃ�
		else {
			return InputState::ERelease; // Release��Ԃ�Ԃ�
		}
	}

	//�O�̃t���[���ŉ�����Ă��Ȃ��Ȃ�
	else {
		//����͉�����Ă���Ȃ�
		if (mCurrentKey[keyCode] & 0x80) {
			return InputState::EPush; // Push��Ԃ�Ԃ�
		}
		//�����������Ă��Ȃ��Ȃ�
		else {
			return InputState::ENone; // None��Ԃ�Ԃ�
		}
	}
}

//------------------------------
//  GamePad�{�^����Ԃ̎擾
//------------------------------
InputState Input::GetButtonState(int button) {
	// �O�̃t���[���ŉ�����Ă�����
	if (mPrevGamePad.wButtons & button) {
		// �����������Ă���Ȃ�
		if (mCurrentGamePad.wButtons & button) {
			return InputState::EHold; // Hold��Ԃ�Ԃ�
		}
		//����͉�����Ă��Ȃ��Ȃ�
		else {
			return InputState::ERelease; // Release��Ԃ�Ԃ�
		}
	}

	//�O�̃t���[���ŉ�����Ă��Ȃ��Ȃ�
	else {
		//����͉�����Ă���Ȃ�
		if (mCurrentGamePad.wButtons & button) {
			return InputState::EPush; // Push��Ԃ�Ԃ�
		}
		//�����������Ă��Ȃ��Ȃ�
		else {
			return InputState::ENone; // None��Ԃ�Ԃ�
		}
	}
}

//---------------------------
//  �X�e�B�b�N���W�̎擾
//---------------------------
DirectX::XMFLOAT2 Input::GetStickAxis() {
	return mLStickAxis;
}

DirectX::XMFLOAT2 Input::GetRStickAxis() {
	return mRStickAxis;
}

//--------------------------------------------
//�@�@�ŐV��key�ƍŐV��keyState�̍X�V
//--------------------------------------------
void Input::UpdateCurrentKey() {
	UpdateGamePadState();
	UpdateKeyboardKeyState();
}


//----------------------------------------
//  GamePad��Ԃ̍X�V
//----------------------------------------
void Input::UpdateGamePadState() {
	XINPUT_STATE s;
	ZeroMemory(&s, sizeof(s));
	if (XInputGetState(0, &s) != ERROR_SUCCESS) return;

	// �f�b�h�]�[���̃R���g���[���[���͂�0��
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

	// ���X�e�B�b�N���͒l��-1 �` 1�ɂ���
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

	// �E�X�e�B�b�N���͒l��-1 �` 1�ɂ���
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
//  �L�[�{�[�h��Ԃ̍X�V
//---------------------------------------
void Input::UpdateKeyboardKeyState() {
	memcpy(mPrevKey, mCurrentKey, sizeof(BYTE) * 256);
	// �ŐV��key�̍X�V
	if (!GetKeyboardState(mCurrentKey)) {
		assert(!"�ŐV�̃L�[�{�[�h�X�e�[�g�̎擾�Ɏ��s");
	}
}

