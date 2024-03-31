#pragma once
#include <memory>
#include <unordered_map>
#include "Component.h"

class AudioComponent : public Component{
public:
	//-----------------------------------------------------------------
	/// [機能] オーディオコンポーネント共通の初期化。
	///        最初のインスタンスを生成する前に必ず呼び出す必要がある。
	//-----------------------------------------------------------------
	static void Init(std::shared_ptr<class AudioSystem> audioSystem);
	explicit AudioComponent(const class GameObject& owner);
	~AudioComponent();

	
	class SoundEvent PlaySoundEvent(const std::string& soundName);  /// 返り値は再生したオーディオを操作できるオブジェクト。

	//------------------------------------
	/// [機能] 再生中のサウンド全て停止
	//------------------------------------
	void StopAllSound();
	void Update()override;

	//---------------------------------------------
	/// [機能] 引数名のオーディオが再生中か調べる
	//---------------------------------------------
	bool IsPlaying(const std::string& name);

private:
	const class GameObject& mOwner;
	std::vector<class SoundEvent> mSoundEvents;
	static std::shared_ptr<class AudioSystem> mAudioSystem;
};

using AudioCompPtrS = std::shared_ptr<AudioComponent>;