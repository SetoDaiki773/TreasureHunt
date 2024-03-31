#pragma once
#include <functional>
#include <DirectXMath.h>

enum StickType {
	ERStick,
	ELStick
};

//=============================================================
/// [�@�\] �X�e�B�b�N�̃^�C�v�ɑ΂���A�N�V�������Ǘ�����B
//=============================================================
class GamepadStickCommand{
public:
	GamepadStickCommand(std::function<void(DirectX::XMFLOAT2 stickAxis)>,StickType stickType);
	//-----------------------------------------------------------
	/// [�@�\] �X�e�B�b�N�̌X�������ƂɃA�N�V�����֐����Ăяo��
	//-----------------------------------------------------------
	void Execute()const;

private:
	std::function<void(DirectX::XMFLOAT2)> mAction;
	StickType mStickType;

};
