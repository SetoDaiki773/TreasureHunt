#include "AudioSystem.h"
#include "Common/VectorOperation.h"
#include <windows.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <vector>
#include <cassert>
#include <string>
#ifdef _DEBUG
#include <iostream>
#endif

bool AudioSystem::mHasInstanced = false;


void AudioSystem::Init() {
	// FMODデバックの初期化
	FMOD::Debug_Initialize(
		FMOD_DEBUG_LEVEL_ERROR,
		FMOD_DEBUG_MODE_TTY
	);
	FMOD_RESULT result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK) {
		assert(!FMOD_ErrorString(result));
	}
	const unsigned numMaxConcurrentSounds = 512;
	result = mSystem->initialize(
		numMaxConcurrentSounds,
		FMOD_STUDIO_INIT_NORMAL,
		FMOD_INIT_NORMAL,
		nullptr
	);
	if (result != FMOD_OK) {
		assert(FMOD_ErrorString(result));
	}

	mSystem->getLowLevelSystem(&mLowLevelSystem);

	LoadBank("FmodStudio/Jikken/Build/Desktop/Master Bank.strings.bank");
	LoadBank("FmodStudio/Jikken/Build/Desktop/Master Bank.bank");
}

AudioSystem::AudioSystem() 
	:mSystem(nullptr), 
	mNextID(0),
	mLowLevelSystem(nullptr) 
{
	if (mHasInstanced) assert(!"AudioSystemのInstanceは一つに制限されています");
	Init();
	mHasInstanced = true;
}

AudioSystem::~AudioSystem() {
	UnloadAllBanks();
	if (mSystem) {
		mSystem->release();
	}
}


void AudioSystem::Destory() {
	
}



//--------------------------------------------------------------------
// バンクをロード。バンクとオーディオファイルの集まりみたいなもの
//--------------------------------------------------------------------
void AudioSystem::LoadBank(const std::string& path) {
	if (mBanks.find(path) != mBanks.end()) {
		return;
	}
	
	FMOD::Studio::Bank* bank = nullptr;  // ここにバンクをロードする
	FMOD_RESULT result = mSystem->loadBankFile(
		path.c_str(),
		FMOD_STUDIO_LOAD_BANK_NORMAL,
		&bank
	);
	assert(result == FMOD_OK);
	constexpr unsigned maxPathLength = 512;
	mBanks.emplace(path,bank); // ロードしたバンクをハッシュに追加
	bank->loadSampleData();    // オーディオデータをバンクにロード
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	if (numEvents > 0) {   // イベントがあるなら
		// イベントディスクリプションは鳴らす前の一つのオーディオ
		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
		bank->getEventList(events.data(),numEvents,&numEvents);  // 配列にイベントディスクリプションを書き込んでいく
		char eventName[maxPathLength];
		// mEventsにeventとイベント名を追加していく
		for (int i = 0; i < numEvents;i++) {
			FMOD::Studio::EventDescription* event = events[i];
			event->getPath(eventName,maxPathLength,nullptr);
#ifdef _DEBUG
			std::cout << eventName << '\n';
#endif
			mEvents.emplace(eventName,event);
		}

	}
	// バスはよくわからん
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0) {  // バスがあるなら
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(),numBuses,&numBuses);
		char busName[maxPathLength];
		// mBusesにbusとバス名を追加していく
		for (int i = 0; i < numBuses;i++) {
			FMOD::Studio::Bus* bus = buses[i];
			bus->getPath(busName,maxPathLength,nullptr);
			mBuses.emplace(busName,bus);
		}
	}


}



//----------------------------
//  バンクをアンロード
//----------------------------
void AudioSystem::UnloadBank(const std::string& name) {
	auto iter = mBanks.find(name);
	if (iter == mBanks.end()) {
		return;
	}
	FMOD::Studio::Bank* bank = iter->second;
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	constexpr unsigned maxPathLength = 512;
	if (numEvents > 0) {
		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
		bank->getEventList(events.data(),numEvents,&numEvents);
		char eventName[maxPathLength];
		for (int i = 0; i < numEvents;i++) {
			FMOD::Studio::EventDescription* event = events[i];
			event->getPath(eventName,maxPathLength,nullptr);
			auto unloadEvent = mEvents.find(eventName);
			// イベントディスクリプションを削除
			if (unloadEvent != mEvents.end()) {
				mEvents.erase(unloadEvent);
			}
		}
	}
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0) {
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(), numBuses, &numBuses);
		char busName[maxPathLength];
		for (int i = 0; i < numBuses; i++) {
			FMOD::Studio::Bus* bus = buses[i];
			bus->getPath(busName, maxPathLength, nullptr);
			auto unloadBus = mBuses.find(busName);
			// バスも削除
			if (unloadBus != mBuses.end()) {
				mBuses.erase(unloadBus);
			}
		}

	}
	bank->unloadSampleData();
	bank->unload();
	mBanks.erase(iter);  // これメモリリークしないの?
}


void AudioSystem::UnloadAllBanks() {
	for (auto& iter : mBanks) {
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	mEvents.clear();
}

void AudioSystem::Update() {
	std::vector<unsigned> doneEventIds;
	for (auto& iter: mEventInstances) {
		FMOD::Studio::EventInstance* eventInstance = iter.second;
		FMOD_STUDIO_PLAYBACK_STATE instanceState;
		eventInstance->getPlaybackState(&instanceState); // プレイイベントの状態を取得して、
		// ストップしたサウンドは終了したサウンド軍に追加
		if (instanceState == FMOD_STUDIO_PLAYBACK_STOPPED) {
			eventInstance->release();
			eventInstance = nullptr;
			doneEventIds.emplace_back(iter.first);
		}
	}
	// 終了したサウンド軍を全部削除
	for (auto id : doneEventIds) {
		
		mEventInstances.erase(id);
	}
	mSystem->update(); // システムのアップデート
}

void AudioSystem::StopAllSound() {
	for (auto& iter : mEventInstances) {
		FMOD_STUDIO_STOP_MODE mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;
		iter.second->stop(mode);
	}

}


namespace {
	FMOD_VECTOR ToFmodVector(const DirectX::XMFLOAT3& vector) {
		FMOD_VECTOR fVector;
		fVector.x = vector.x;
		fVector.y = vector.y;
		fVector.z = vector.z;
		return fVector;
	}
}


void AudioSystem::SetListenerPos(const DirectX::XMFLOAT3& listenerPos,const DirectX::XMFLOAT3& lookPos) {
	/*-----------------------------------------------
	   注視位置のy軸を入れるとなぜか音の増減の仕方
	   がおかしくなるからとりあえず、除いておいてる
	-------------------------------------------------*/
	FMOD_3D_ATTRIBUTES listener;
	DirectX::XMFLOAT3 look(lookPos.x,0.0f,lookPos.z); // 上述の通り、y成分は0にする。
	listener.position = ToFmodVector(listenerPos);
	listener.forward = ToFmodVector(Vector::GetNormalize(look));
	listener.up = {0.0f,1.0f,0.0f};
	listener.velocity = { 0.0f,0.0f,0.0f };  // ドップラー効果を使うならいじる。
	mSystem->setListenerAttributes(0,&listener);
}


SoundEvent AudioSystem::PlayEvent(const std::string& name) {
	auto iter = mEvents.find(name);
	FMOD::Studio::EventInstance* eventInstance = nullptr;
	assert(iter != mEvents.end());
	if (iter != mEvents.end()) {
		iter->second->createInstance(&eventInstance);
		if (eventInstance) {
			eventInstance->start();
			mNextID++;
			mEventInstances.emplace(mNextID, eventInstance);
		}
	}
	// イベントを操るためのサウンドイベントというインスタンスを返す
	return SoundEvent(*eventInstance,name);
}






//------------------------------    ここからは SoundEvent  クラス    --------------------------------


SoundEvent::SoundEvent(FMOD::Studio::EventInstance& event,const std::string& eventName)
	:mEvent(event)
	,mEventName(eventName)
{

}

SoundEvent& SoundEvent::operator=(const SoundEvent& soundEvent) 
{
	mEvent = soundEvent.mEvent;
	return *this;
}


bool SoundEvent::Is3D()const {
	bool is3D = false;
	if (mEvent.isValid()) {
		FMOD::Studio::EventDescription* ed = nullptr;
		mEvent.getDescription(&ed);
		if (ed) {
			ed->is3D(&is3D);
		}
	}
	return is3D;
}

bool SoundEvent::IsValid() {
	if (mEvent.isValid()) return true;
	return false;
}

void SoundEvent::Set3DAttributes(const DirectX::XMFLOAT3& audioPos) {
	if (mEvent.isValid()) {
		FMOD_3D_ATTRIBUTES audio3D;
		audio3D.position = ToFmodVector(audioPos);

		// 現状音源の方向はどうでも良いから適当に入れておく
		audio3D.forward = { 0.0f,0.0f,1.0f };
		audio3D.up = { 0.0f,1.0f,0.0f };
		audio3D.velocity = { 0.0f,0.0f,0.0f }; // ドップラー効果が必要ならいじる。
		mEvent.set3DAttributes(&audio3D);
	}
}


void SoundEvent::Restart() {
	mEvent.start();
}


void SoundEvent::Stop() {
	if (mEvent.isValid()) {
		FMOD_STUDIO_STOP_MODE mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;
		mEvent.stop(mode);
	}
}


void SoundEvent::SetPaused() {
	if (mEvent.isValid()) {
		mEvent.setPaused(true);
	}
}


void SoundEvent::SetVolume(float volume) {
	if (mEvent.isValid()) {
		mEvent.setVolume(volume);
	}
}


void SoundEvent::SetPitch(float pitch) {
	if (mEvent.isValid()) {
		mEvent.setPitch(pitch);
	}
}


void SoundEvent::SetParameter(const std::string& name,float parameter) {
	if (mEvent.isValid()) {
		mEvent.setParameterValue(name.c_str(),parameter);
	}
}


bool SoundEvent::GetPaused()const {
	bool isPaused = true;
	if (mEvent.isValid()) {
		mEvent.getPaused(&isPaused);
	}
	return isPaused;
}


float SoundEvent::GetVolume()const {
	float volume = 1.0f;
	if (mEvent.isValid()) {
		mEvent.getVolume(&volume);
	}
	return volume;
}


float SoundEvent::GetPitch()const {
	float pitch = 1.0f;
	if (mEvent.isValid()) {
		mEvent.getPitch(&pitch);
	}
	return pitch;
}


float SoundEvent::GetParameter(const std::string& name)const {
	float parameter = 0.0f;
	if (mEvent.isValid()) {
		mEvent.getParameterValue(name.c_str(), &parameter);
	}
	return parameter;
}

std::string SoundEvent::GetName() {
	return mEventName;
}




