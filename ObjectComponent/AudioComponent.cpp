#include "AudioComponent.h"
#include "Common/AudioSystem.h"
#include "GameObject/GameObject.h"

std::shared_ptr<AudioSystem> AudioComponent::mAudioSystem;

void AudioComponent::Init(std::shared_ptr<AudioSystem> audioSystem) {
	mAudioSystem = audioSystem;
}


AudioComponent::AudioComponent(const GameObject& owner)
	:mOwner(owner)
{
	assert(mAudioSystem);
}

AudioComponent::~AudioComponent() {
	
}

void AudioComponent::Update() {
	// �Đ����I�������͏���
	auto iter = mSoundEvents.begin();
	while (iter != mSoundEvents.end())
	{
		// �����A�܂�Đ����I����Ă���Ȃ�
		if (!iter->IsValid())
		{
			iter = mSoundEvents.erase(iter); // ����
		}
		else
		{
			++iter;
		}
	}

	// �S�Ă�3D�T�E���h�̈ʒu�X�V
	for (auto& soundEvent : mSoundEvents) {
		//if (!soundEvent.Is3D()) continue;
		soundEvent.Set3DAttributes(mOwner.GetPos());
	}
}

//------------------------------------------------------------
//  �����̃T�E���h���̃T�E���h���Đ��B�T�E���h�C�x���g��Ԃ�
//------------------------------------------------------------
SoundEvent AudioComponent::PlaySoundEvent(const std::string& soundName) {
	mSoundEvents.push_back(mAudioSystem->PlayEvent(soundName));
	return mSoundEvents.back();
}

//------------------------------------
//  �S�ẴT�E���h�̒�~
//------------------------------------
void AudioComponent::StopAllSound() {
	for (auto& soundEvent : mSoundEvents) {
		soundEvent.Stop();
	}
}

//------------------------------------
//  �����̃T�E���h���Đ�������Ԃ�
//------------------------------------
bool AudioComponent::IsPlaying(const std::string& name) {
	// �����ƈ�v����v���C���̃I�[�f�B�I����������true��Ԃ�
	for (auto& soundEvent : mSoundEvents) {
		if (soundEvent.GetName() != name) continue;
		return true;
	}
	return false;
}

