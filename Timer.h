#pragma once
#include <memory>
#include <windows.h>
#include <DirectXMath.h>

//===========================================================================================================
/// [�@�\] �R���X�g���N�^���Ăяo���ƃ^�C�}�[�X�^�[�g�B�~�߂���A�`�悵����A���ݎ��Ԃ��擾������ł���
//===========================================================================================================
class Timer {
public:
	explicit Timer(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(0.1f,0.1f,0.1f),DWORD startTime = 0);
	~Timer();

	//------------------------------------------
	//  [�@�\]�^�C�}�[�̌��ݎ��ԕ\���B�P�ʂ͕b
	//------------------------------------------
	void Render(const class Camera& camera);

	//----------------------------------
	// [�@�\] ���ݎ��Ԏ擾�B�P�ʂ͕b
	//----------------------------------
	DWORD GetTime();

	//------------------------------
	// [�@�\] �^�C�}�[���~�߂�
	//------------------------------
	void Stop();

	//-------------------------------------
	// [�@�\] �~�߂��^�C�}�[���Ăѓ�����
	//-------------------------------------
	void ReleaseStop();
	

private:
	std::unique_ptr<class NumberUI> mNumberUI;
	DWORD mStartTime;             // timer���J�n��������timeGetTime()���Ԃ����l
	unsigned mCumulativeStopTime; // �݌v�X�g�b�v����
	unsigned mStopStartTime;      // �^�C�}�[���~�������̊J�n����
	bool mIsStop;
};
