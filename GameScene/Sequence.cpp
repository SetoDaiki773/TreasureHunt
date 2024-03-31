#include "Sequence.h"
#include "Title.h"
#include "Stage.h"
#include "StageCreater.h"
#include "NewScene.h"
#include <cassert>
#include <iostream>
#include <Windows.h>


bool Sequence::mIsRunTime = false;

Sequence::Sequence() 
	:mScene(new Title{})
{
	mIsRunTime = false;
}

Sequence::~Sequence() {
	delete mScene;
}

//--------------------------------------------
//  ���������^�C�����ǂ���
//--------------------------------------------
bool Sequence::IsRunTime() {
	return mIsRunTime;
}


//--------------------------------------------
//   �V�[���ɉ������X�V�������Ăяo���܂�
//--------------------------------------------
void Sequence::RunScene() {
	Scene* nextScene = mScene->Run();

	// �O�t���[���ƌ��݂̃Q�[���V�[�����ς���Ă���mScene��delete����Scene������
	if (nextScene != mScene) {
		mIsRunTime = false;
		delete mScene;
		mScene = nextScene;
		mIsRunTime = true;
	}
}

