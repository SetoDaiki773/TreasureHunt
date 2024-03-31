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
//  今がランタイムかどうか
//--------------------------------------------
bool Sequence::IsRunTime() {
	return mIsRunTime;
}


//--------------------------------------------
//   シーンに応じた更新処理を呼び出します
//--------------------------------------------
void Sequence::RunScene() {
	Scene* nextScene = mScene->Run();

	// 前フレームと現在のゲームシーンが変わってたらmSceneをdeleteしてSceneを交換
	if (nextScene != mScene) {
		mIsRunTime = false;
		delete mScene;
		mScene = nextScene;
		mIsRunTime = true;
	}
}

