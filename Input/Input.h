#pragma once
#include <windows.h>
#include <XInput.h>
#include <DirectXMath.h>

#pragma comment(lib,"xinput.lib")

//===============================================
//  key�̏�ԗ񋓌^
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
	/// [�@�\] �ŐV��key��State�̎擾����
	/// [����] XInput�ɒ�`���ꂽkey�̖��O(���Ԃ�int�^�ł��邾��)
	//-------------------------------------------------------------------
	static InputState GetKeyState(int keyCode);

	//-------------------------------------------------------------------
	/// [�@�\] �ŐV��key��State�̎擾
	/// [����] XInput�ɒ�`���ꂽkey�̖��O(���Ԃ�int�^�ł��邾��)
	//-------------------------------------------------------------------
	static InputState GetButtonState(int button);
	//-----------------------------------------------
	/// [�@�\] �X�e�B�b�N�̌X����Ԃ��B
	//-----------------------------------------------
	static DirectX::XMFLOAT2 GetStickAxis();
	static DirectX::XMFLOAT2 GetRStickAxis();

	//-------------------------------------------------------------------------------
	/// [�@�\] �ŐV�̉����ꂽkey��keyState�ƍŐV�̉����ꂽkye���X�V���܂�
	/// [�g�p���@/���ӓ_] �C���X�^���X�̐��֌W�Ȃ����̃��[�v�ɂ���x�����Ăяo���܂�
	//-------------------------------------------------------------------------------
	static void UpdateCurrentKey();

private:
	Input();    // �V���O���g���Ȃ̂ŃR���X�g���N�^�[����
	~Input();   // �V���O���g���Ȃ̂Ńf�X�g���N�^�[����
	Input(const Input&) = delete;  // �V���O���g���Ȃ̂ŃR�s�[�R���X�g���N�^�[����


	static void UpdateGamePadState();
	static void UpdateKeyboardKeyState();
	static BYTE mPrevKey[256];
	static BYTE mCurrentKey[256];
	static XINPUT_GAMEPAD mPrevGamePad;
	static XINPUT_GAMEPAD mCurrentGamePad;
	static DirectX::XMFLOAT2 mRStickAxis;
	static DirectX::XMFLOAT2 mLStickAxis;
};

