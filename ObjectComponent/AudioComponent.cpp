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
	// 再生し終わったやつは消す
	auto iter = mSoundEvents.begin();
	while (iter != mSoundEvents.end())
	{
		// 無効、つまり再生し終わっているなら
		if (!iter->IsValid())
		{
			iter = mSoundEvents.erase(iter); // 消す
		}
		else
		{
			++iter;
		}
	}

	// 全ての3Dサウンドの位置更新
	for (auto& soundEvent : mSoundEvents) {
		//if (!soundEvent.Is3D()) continue;
		soundEvent.Set3DAttributes(mOwner.GetPos());
	}
}

//------------------------------------------------------------
//  引数のサウンド名のサウンドを再生。サウンドイベントを返す
//------------------------------------------------------------
SoundEvent AudioComponent::PlaySoundEvent(const std::string& soundName) {
	mSoundEvents.push_back(mAudioSystem->PlayEvent(soundName));
	return mSoundEvents.back();
}

//------------------------------------
//  全てのサウンドの停止
//------------------------------------
void AudioComponent::StopAllSound() {
	for (auto& soundEvent : mSoundEvents) {
		soundEvent.Stop();
	}
}

//------------------------------------
//  引数のサウンドが再生中かを返す
//------------------------------------
bool AudioComponent::IsPlaying(const std::string& name) {
	// 引数と一致するプレイ中のオーディオがあったらtrueを返す
	for (auto& soundEvent : mSoundEvents) {
		if (soundEvent.GetName() != name) continue;
		return true;
	}
	return false;
}

