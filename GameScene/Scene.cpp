#include"Scene.h"
#include "Sequence.h"
#include "ObjectComponent/AudioComponent.h"
#include "Common/AudioSystem.h"

std::shared_ptr<AudioSystem> Scene::mAudioSystem;

Scene::Scene()
{
	static bool isFirst = true;
	if (isFirst) {
		mAudioSystem = std::make_shared<AudioSystem>();
		AudioComponent::Init(mAudioSystem);
		isFirst = false;
	}
}

Scene::~Scene() {
	
}
//--------------------------------------------
//   シーンを開始します
//--------------------------------------------
Scene* Scene::Run() {
	Scene* scene = Update();
	Render();
	return scene;
}