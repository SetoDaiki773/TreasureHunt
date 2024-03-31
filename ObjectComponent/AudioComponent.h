#pragma once
#include <memory>
#include <unordered_map>
#include "Component.h"

class AudioComponent : public Component{
public:
	//-----------------------------------------------------------------
	/// [�@�\] �I�[�f�B�I�R���|�[�l���g���ʂ̏������B
	///        �ŏ��̃C���X�^���X�𐶐�����O�ɕK���Ăяo���K�v������B
	//-----------------------------------------------------------------
	static void Init(std::shared_ptr<class AudioSystem> audioSystem);
	explicit AudioComponent(const class GameObject& owner);
	~AudioComponent();

	
	class SoundEvent PlaySoundEvent(const std::string& soundName);  /// �Ԃ�l�͍Đ������I�[�f�B�I�𑀍�ł���I�u�W�F�N�g�B

	//------------------------------------
	/// [�@�\] �Đ����̃T�E���h�S�Ē�~
	//------------------------------------
	void StopAllSound();
	void Update()override;

	//---------------------------------------------
	/// [�@�\] �������̃I�[�f�B�I���Đ��������ׂ�
	//---------------------------------------------
	bool IsPlaying(const std::string& name);

private:
	const class GameObject& mOwner;
	std::vector<class SoundEvent> mSoundEvents;
	static std::shared_ptr<class AudioSystem> mAudioSystem;
};

using AudioCompPtrS = std::shared_ptr<AudioComponent>;