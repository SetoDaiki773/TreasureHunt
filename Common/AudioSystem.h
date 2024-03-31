#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#pragma comment(lib,"fmodL64_vc.lib")
#pragma comment(lib,"fmodstudioL64_vc.lib")

namespace FMOD
{
	class System;
	namespace Studio
	{
		class Bank;
		class EventDescription;
		class EventInstance;
		class System;
		class Bus;
	};
};

//================================================================================================
/// [機能] 鳴らしたオーディオを操るためのクラス。このクラスはオーディオを鳴らす際に返される
//================================================================================================
class SoundEvent {
public:
	SoundEvent(FMOD::Studio::EventInstance& event,const std::string& evetName);
	SoundEvent& operator=(const SoundEvent&);
	bool Is3D()const;
	bool IsValid();
	void Set3DAttributes(const DirectX::XMFLOAT3& audioPos);
	void Restart();
	void Stop();
	void SetPaused();
	void SetVolume(float volume);
	void SetPitch(float pitch);
	void SetParameter(const std::string& name, float parameter);
	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name)const;
	std::string GetName();

private:
	FMOD::Studio::EventInstance& mEvent;
	std::string mEventName;
};

class AudioSystem {
public:
	AudioSystem();
	~AudioSystem();
	void Destory();
	void LoadBank(const std::string& path);
	void UnloadBank(const std::string& name);
	void UnloadAllBanks();
	void SetListenerPos(const DirectX::XMFLOAT3& listenerPos,const DirectX::XMFLOAT3& listenrQuaternion);
	void Update();
	void StopAllSound();

	//-----------------------------------------------------------------------------------------
	/// [機能] nameと一致したイベントを鳴らして、そのサウンドを操るためのSoundEventを返す
	//-----------------------------------------------------------------------------------------
	SoundEvent PlayEvent(const std::string& name);

private:
	void Init();

	
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	std::unordered_map<unsigned, FMOD::Studio::EventInstance*> mEventInstances;
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
	FMOD::Studio::System* mSystem;
	FMOD::System* mLowLevelSystem;
	int mNextID;
	static bool mHasInstanced;
	AudioSystem(AudioSystem&) = delete;
	

};
