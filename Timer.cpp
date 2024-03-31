#include "Timer.h"
#include "Number.h"
#include "Camera/Camera.h"
#include <iostream>

Timer::Timer(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale,DWORD startTime)
	:mStartTime(timeGetTime() - (startTime * 1000))
	,mNumberUI(new NumberUI(pos, scale))
	,mCumulativeStopTime(0)
	,mStopStartTime(0)
	,mIsStop(false)
{

}

Timer::~Timer() {

}


//----------------------------
//  時間取得
//----------------------------
DWORD Timer::GetTime() {
	// 仮に10秒でタイマーを停止して0秒からタイマーをスタートしており、今までで累計5秒タイマーが止まっていたら
	// 10 - 0 - 5 = 5秒となる。
	if (mIsStop) {
		return (mStopStartTime - mStartTime - mCumulativeStopTime) /1000;   // 秒にするため1000割り。もとはmsec
	}
	// 仮にtimeGetTime()が10で0秒からスタート、累計5秒止まっていたら
	// 10 - 0 - 5 = 5秒となる
	// timeGetTime()はコンストラクタ呼び出しからの経過時間を返すと思えば問題ない。(正確には違う)
	return (timeGetTime() - mStartTime - mCumulativeStopTime) / 1000;
}

//-----------------------------
//  タイマー停止
//-----------------------------
void Timer::Stop() {
	if (mIsStop) return;
	mStopStartTime = timeGetTime();
	mIsStop = true;
}

//-------------------------------
// タイマー停止解除
//-------------------------------
void Timer::ReleaseStop() {
	if (!mIsStop) return;
	
	mCumulativeStopTime += ((timeGetTime() - mStopStartTime) / 1000);  // 累計停止時間を停止開始時間をもとに加算
	mStopStartTime = 0;
	mIsStop = false;
}

//------------------------------
//  タイマー描画
//------------------------------
void Timer::Render(const class Camera& camera) {
	mNumberUI->Render(camera, GetTime());
}

