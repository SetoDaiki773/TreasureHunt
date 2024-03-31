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
//  ���Ԏ擾
//----------------------------
DWORD Timer::GetTime() {
	// ����10�b�Ń^�C�}�[���~����0�b����^�C�}�[���X�^�[�g���Ă���A���܂łŗ݌v5�b�^�C�}�[���~�܂��Ă�����
	// 10 - 0 - 5 = 5�b�ƂȂ�B
	if (mIsStop) {
		return (mStopStartTime - mStartTime - mCumulativeStopTime) /1000;   // �b�ɂ��邽��1000����B���Ƃ�msec
	}
	// ����timeGetTime()��10��0�b����X�^�[�g�A�݌v5�b�~�܂��Ă�����
	// 10 - 0 - 5 = 5�b�ƂȂ�
	// timeGetTime()�̓R���X�g���N�^�Ăяo������̌o�ߎ��Ԃ�Ԃ��Ǝv���Ζ��Ȃ��B(���m�ɂ͈Ⴄ)
	return (timeGetTime() - mStartTime - mCumulativeStopTime) / 1000;
}

//-----------------------------
//  �^�C�}�[��~
//-----------------------------
void Timer::Stop() {
	if (mIsStop) return;
	mStopStartTime = timeGetTime();
	mIsStop = true;
}

//-------------------------------
// �^�C�}�[��~����
//-------------------------------
void Timer::ReleaseStop() {
	if (!mIsStop) return;
	
	mCumulativeStopTime += ((timeGetTime() - mStopStartTime) / 1000);  // �݌v��~���Ԃ��~�J�n���Ԃ����Ƃɉ��Z
	mStopStartTime = 0;
	mIsStop = false;
}

//------------------------------
//  �^�C�}�[�`��
//------------------------------
void Timer::Render(const class Camera& camera) {
	mNumberUI->Render(camera, GetTime());
}

