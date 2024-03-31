#pragma once
#include <memory>
#include <windows.h>
#include <DirectXMath.h>

//===========================================================================================================
/// [機能] コンストラクタを呼び出すとタイマースタート。止めたり、描画したり、現在時間を取得したりできる
//===========================================================================================================
class Timer {
public:
	explicit Timer(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(0.1f,0.1f,0.1f),DWORD startTime = 0);
	~Timer();

	//------------------------------------------
	//  [機能]タイマーの現在時間表示。単位は秒
	//------------------------------------------
	void Render(const class Camera& camera);

	//----------------------------------
	// [機能] 現在時間取得。単位は秒
	//----------------------------------
	DWORD GetTime();

	//------------------------------
	// [機能] タイマーを止める
	//------------------------------
	void Stop();

	//-------------------------------------
	// [機能] 止めたタイマーを再び動かす
	//-------------------------------------
	void ReleaseStop();
	

private:
	std::unique_ptr<class NumberUI> mNumberUI;
	DWORD mStartTime;             // timerを開始した時にtimeGetTime()が返した値
	unsigned mCumulativeStopTime; // 累計ストップ時間
	unsigned mStopStartTime;      // タイマーを停止した時の開始時間
	bool mIsStop;
};
