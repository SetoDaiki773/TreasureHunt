#include "Stage.h"
#include "Input/Input.h"
#include "Common/VectorOperation.h"
#include "Common/QuaternionOperation.h"
#include "ObjectComponent/Model.h"
#include "ObjectComponent/WorldTransform.h"
#include "GameObject/GameObject.h"
#include "ObjectComponent/PlayerInputComponent.h"
#include "Camera/Camera.h"
#include "Camera/TPSCamera.h"
#include "Scene/Sequence.h"
#include "ObjectComponent/AudioComponent.h"
#include "Common/AudioSystem.h"
#include "ObjectComponent/RigidBody.h"
#include "ObjectComponent/PlayerBehavior.h"
#include "ObjectComponent/KaeruBehavior.h"
#include "ObjectComponent/BarrelBehavior.h"
#include "Number.h"
#include "Timer.h"
#include "Manager/ModelManager.h"
#include "StageSelect.h"
#include "Json/JsonFile.h"
#include "StageCreater.h"
#include "KeyCounter.h"
#include "KeyAndLockColor.h"
#include <cmath>

using namespace DirectX;

enum class ObjType {
	Person,
	Kaeru,
	Cube,
	Sky,
	Floor,
	Other
};

Stage::Stage(std::unique_ptr<StageCreater> stageCreater,const std::string& jsonStageObjectName)
	:mCamera(nullptr)
	,mTimer(nullptr)
	,mGameOverSound(nullptr)
	,mModelManager(std::make_unique<ModelManager>())
	,mMedal(Medal::ENothing)
	,mJsonFile(std::make_unique<JsonFile>("Json/StageInfo.json"))
	,mJsonStageObjectName(jsonStageObjectName)
	,mStageCreater(std::move(stageCreater))
	,mKeyCounter(std::make_unique<KeyCounter>())
	,mIsClear(false)
	,mWasCreatedResult(false)
	,mElapsedTimeAfterClear(0)
{
	LoadModels();
	mAudioSystem->StopAllSound();
	mBGMSound = std::make_unique<SoundEvent>(mAudioSystem->PlayEvent("event:/walking_bgm"));
	mStageCreater->CreateStage(*this);
	// ロード時間を経過時間に含めないために、最後にnewする
	mTimer = std::make_unique<Timer>(XMFLOAT3(-0.1f, 0.87f, 0.3f), XMFLOAT3(0.08f, 0.08f, 0.08f));
}

Stage::~Stage() {
	mGameObjects.clear();
}



//------------------------------------
//  シーンの更新
//------------------------------------
Scene* Stage::Update() {
	PhysXWorld::Update();  // 物理世界を更新する
	DynamicRigidBody::ReflectAllPosOfPhysXWorld(); // PhysXの上のオブジェクトの位置をゲーム世界に反映する
	StaticRigidBody::ReflectAllPosOfPhysXWorld();  // staticも同じ
	mAudioSystem->Update();
	mAudioSystem->SetListenerPos(mCamera->GetPos(),mCamera->GetLookPos());
	Input::UpdateCurrentKey();
	mCamera->Update();
	// 全てのオブジェクトを更新
	for (auto objs : mGameObjects) {
		for (auto obj : objs.second) {
			obj->Update();
		}
	}
	// Dead状態のオブジェクトを消す
	for (auto& gameObjects : mGameObjects) {
		std::erase_if(gameObjects.second, [](GameObjectPtrS gameObject) { return gameObject->GetState() == GameObject::State::EDead; });
	}
	

	Scene* next = this;  // 次のシーンはとりあえず今と同じものにしておく
	// クリアしていたらリザルトを出す。180秒経ってからStageSelectへ移動する
	if (mIsClear) {
		CreateResult();
		mElapsedTimeAfterClear++;
		// クリアしてから180フレーム経過したら次のシーンをStageSelectへ変更
		if (mElapsedTimeAfterClear >= 180.0f) {
			SaveCrearTimeAndMedal();  // セーブをしておく
			next = new StageSelect(); // 次のシーンはステージセレクト画面
		}
	}

	// プレイヤーが落下したらBGMを停止して、GameOverBGMをならす。GameOverBGMが止まったら、リスタート。

	// プレイヤーが落下したら
	if (mGameObjects[ObjType::Person][0]->GetPos().y <= -10.0f) {
		// GameOver用のサウンドがなっていなら鳴らす。
		if (mGameOverSound == nullptr) {
			mGameOverSound = std::make_unique<SoundEvent>(mAudioSystem->PlayEvent("event:/GameOver"));
		}
		mGameObjects[ObjType::Person][0]->SetState(GameObject::State::EStopUndraw);
		mBGMSound->Stop();
		// GameOberサウンドが止まったら
		if (!mGameOverSound->IsValid()) {
			next = new Stage(std::move(mStageCreater), mJsonStageObjectName);  // 同じシーンを初期化する。つまりリスタート
		}
	}


	// Rをおしたらステージ状態をリセットする
	if (Input::GetKeyState('R') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_RIGHT_SHOULDER) == InputState::EPush) {
		next = new Stage(std::move(mStageCreater), mJsonStageObjectName); // 本質的には今と同じステージをnewする
	}
	
	// Qを押したらステージセレクトへ
	if (Input::GetKeyState('Q') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_LEFT_SHOULDER) == InputState::EPush) {
		next = new StageSelect(); // 次のシーンはステージセレクト
	}

	DynamicRigidBody::ReflectAllPosToPhysXWorld();  // オブジェクトの更新が終わったのでゲーム世界の位置を物理世界に反映させる
	StaticRigidBody::ReflectAllPosToPhysXWorld();   // staticも同じ
	

	return next;
}


//------------------------------
//  シーン描画
//------------------------------
void Stage::Render() {
	NumberUI redKeyCountUI(XMFLOAT3(0.35f, 0.87f, 0.3f), XMFLOAT3(0.08f, 0.08f, 0.08f));
	NumberUI blueKeyCountUI(XMFLOAT3(0.6f, 0.87f, 0.3f), XMFLOAT3(0.08f, 0.08f, 0.08f));
	// 全オブジェクトとタイマー、鍵の数を描画
	Model::BeginRender();
	for (auto objs : mGameObjects) {
		for (auto obj : objs.second) {
			obj->Render(*mCamera);
		}
	}
	redKeyCountUI.Render(*mCamera, mKeyCounter->redKeyCount);
	blueKeyCountUI.Render(*mCamera, mKeyCounter->blueKeyCount);
	mTimer->Render(*mCamera);
	Model::EndRender();
}

//-----------------------------------------
//  ゲームオブジェクトをハッシュに追加
//-----------------------------------------
void Stage::AddGameObject(ObjType objType, GameObjectPtrS obj) {
	mGameObjects[objType].push_back(obj);
}



//-----------------------------------
//  人作成
//-----------------------------------
void Stage::CreatePerson(const XMFLOAT3& pos) {
	// モデルとワールド変換情報からプレイヤーの元を作成
	std::shared_ptr<Model> personModel = std::make_shared<Model>(L"Asset/boy.obj", L"Shader/Phong.hlsl", L"Shader/Phong.hlsl");
	WorldTransform world(pos,XMFLOAT3(1.5f,1.8f,1.5f));
	GameObjectPtrS person = std::make_shared<GameObject>(personModel, std::move(world));

	// プレイヤーを注視するTPSカメラの追加
	std::unique_ptr<CameraMovement> testCamera(new TPSCamera(person.get(),0.0f,180.0f));
	mCamera = std::make_unique<Camera>(XMFLOAT3(0.0f, 0.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), std::move(testCamera));

	// 物理要素の追加
	std::shared_ptr<DynamicRigidBody> physics = std::make_shared<DynamicRigidBody>(*person,1.0f);
	physics->AttachBox(0.5f, 0.9f, 0.5f, {1.0f,1.0f,0.0f},"Person");
	physics->AttachBox(0.48f, 0.2f, 0.48f, {}, "PersonHead",XMFLOAT3(0.0f,0.8f,0.0f));
	physics->AttachBoxTrigger(0.48f, 0.3f, 0.48f, {},"Foot",XMFLOAT3(0.0f,-1.0f,0.0f));
	person->AddSharedComponent(physics);

	// オーディオ要素追加
	std::shared_ptr<AudioComponent> audio = std::make_shared<AudioComponent>(*person);
	person->AddSharedComponent(audio);

	// 入力処理要素の追加
	std::shared_ptr<PlayerInputComponent> pInputComp(std::make_shared<PlayerInputComponent>(*person, mCamera.get(),audio.get(),physics.get(), 11.45f, true));
	person->AddSharedComponent(pInputComp);

	// プレイヤーの行動要素の追加
	std::shared_ptr<PlayerBehavior> behavior = std::make_shared<PlayerBehavior>(*person, physics,audio);
	person->AddSharedComponent(behavior);
	
	AddGameObject(ObjType::Person, person);  // ハッシュに追加
}


//-------------------------------------------
//   カエル作成
//-------------------------------------------
void Stage::CreateKaeru(const XMFLOAT3& pos) {
	// カエルの元を作成
	std::shared_ptr<Model> kaeruModel = std::make_shared<Model>(L"Asset/kaeru.obj", L"Shader/Phong.hlsl", L"Shader/Phong.hlsl");
	WorldTransform kaeruWorld(pos,XMFLOAT3(1.5f,1.5f,1.5f));
	GameObjectPtrS kaeru = std::make_shared<GameObject>(kaeruModel, std::move(kaeruWorld));

	// 物理要素の追加
	std::shared_ptr<DynamicRigidBody> phy = std::make_shared<DynamicRigidBody>(*kaeru,0.1f);
	phy->AttachBox(0.6f, 0.6f, 0.6f, {},"Kaeru");
	phy->AttachBoxTrigger(0.3f, 0.3f, 0.3f, {}, "KaeruFoot", XMFLOAT3(0.0f, -0.6f, 0.0f));
	kaeru->AddSharedComponent(phy);

	// オーディオ要素追加
	std::shared_ptr<AudioComponent> audio = std::make_shared<AudioComponent>(*kaeru);
	kaeru->AddSharedComponent(audio);

	// 入力処理要素の追加
	std::shared_ptr<PlayerInputComponent> kaeruInput(std::make_shared<PlayerInputComponent>(*kaeru, mCamera.get(),audio.get(),phy.get(), 14.5f, false));
	kaeru->AddSharedComponent(kaeruInput);

	// カエルの行動要素の追加
	std::shared_ptr<KaeruBehavior> behavior = std::make_shared<KaeruBehavior>(*kaeru,phy,mGameObjects[ObjType::Person][0],kaeruInput,audio);
	kaeru->AddSharedComponent(behavior);
	AddGameObject(ObjType::Kaeru, kaeru); // ハッシュに追加
}


//------------------------------------
//  スカイマップ作製
//------------------------------------
void Stage::CreateSkyMap() {
	// skyマップの作成
	std::shared_ptr<Model> skyModel = std::make_shared<Model>(L"Asset/Sky_Sphere_R10.obj", L"Shader/Phong.hlsl");
	XMVECTOR quaternion = Quaternion::RotateEuler(180.0f, 0.0f, 0.0f);
	WorldTransform world2(XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), quaternion);
	GameObjectPtrS sky = std::make_shared<GameObject>(skyModel, std::move(world2));
	AddGameObject(ObjType::Sky, sky);  // hashに追加
}

//-------------------------------
//  鍵作成
//-------------------------------
void Stage::CreateKey(const XMFLOAT3& pos,Color color) {
	std::map <Color, std::wstring> colorToShaderName{
		{Color::ERed,L"Shader/RedKey.hlsl"},
		{Color::EBlue,L"Shader/BlueKey.hlsl"}
	};
	std::wstring shaderName = colorToShaderName[color];
	WorldTransform world(pos);
	ModelPtrS model = std::make_shared<Model>(L"Asset/Key.obj", shaderName);
	GameObjectPtrS key = std::make_shared<GameObject>(model, std::move(world));
	StaticRigidPtr rigidBody = std::make_shared<StaticRigidBody>(*key);
	rigidBody->AttachBoxTrigger(1.0f, 3.0f, 1.0f, {},"Key");
	GameObject* keyP = key.get();
	auto addKeyCount = [this,color,keyP](const HitObjectInfo& info) {
		if (info.name != "Person" && info.name != "Kaeru") return;
		if (info.triggerEvent != TriggerEvent::EEnter) return;

		std::map<Color, unsigned*> colorToKeyCountMap; // キーカラーからその色のカギの所持数をアクセスするためのマップ
		colorToKeyCountMap[Color::ERed] = &mKeyCounter->redKeyCount;
		colorToKeyCountMap[Color::EBlue] = &mKeyCounter->blueKeyCount;
		auto iter = colorToKeyCountMap.find(color); // 自分の色と同じ鍵を
		*(iter->second) = *(iter->second) + 1;       // 一つ増やす
		keyP->SetState(GameObject::State::EDead);   // 鍵は取得されたので死ぬ
		mAudioSystem->PlayEvent("event:/GetKey");
	};
	rigidBody->SetTriggerFunc(addKeyCount,"Key");
	key->AddSharedComponent(rigidBody);
	AddGameObject(ObjType::Other, key);
}

//------------------------------------
//  ロック作成
//------------------------------------
void Stage::CreateLock(const XMFLOAT3& pos,Color color) {
	std::map <Color, std::wstring> colorToTextureName{
		{Color::ERed,L"Asset/RedLock.jpg"},
		{Color::EBlue,L"Asset/BlueLock.jpg"}
	};
	std::wstring textureName = colorToTextureName[color];  // mapからテクスチャ名を決定
	WorldTransform world(pos); 
	ModelPtrS model = std::make_shared<Model>(Shape::Cube,textureName,L"Shader/NoLighting.hlsl"); // Lockはキューブ型
	GameObjectPtrS lock = std::make_shared<GameObject>(model,std::move(world));

	// 剛体要素
	StaticRigidPtr rigidBody = std::make_shared<StaticRigidBody>(*lock);
	rigidBody->AttachBox(1.0f, 1.0f, 1.0f, {});
	rigidBody->AttachBoxTrigger(1.1f, 1.1f, 1.1f, {},"LockTrigger");
	GameObject* lockP = lock.get();

	// ロックを解除する関数
	auto releaseLock = [this,color,lockP](const HitObjectInfo& info) {
		if (info.triggerEvent == TriggerEvent::EOut) return;
		std::map<Color, unsigned*> colorToKeyCountMap; // キーカラーからその色のカギの所持数をアクセスするためのマップ
		colorToKeyCountMap[Color::ERed] = &mKeyCounter->redKeyCount;
		colorToKeyCountMap[Color::EBlue] = &mKeyCounter->blueKeyCount;
		// 人に当たって、ロックの色に対応した鍵の数が0より大きいなら。
		if ((info.name == "Person") && *(colorToKeyCountMap[color]) > 0) {
			auto iter = colorToKeyCountMap.find(color);
			*(iter->second) = *(iter->second) - 1;      // 鍵を一つ減らして
			lockP->SetState(GameObject::State::EDead);  // ロック解除
			mAudioSystem->PlayEvent("event:/ReleaseLock");
		}
	};
	rigidBody->SetTriggerFunc(releaseLock,"LockTrigger");  // トリガーに関数をセット
	lock->AddSharedComponent(rigidBody);

	AddGameObject(ObjType::Other,lock);
}

//------------------------------------
//  樽作成
//------------------------------------
void Stage::CreateBarrel(const DirectX::XMFLOAT3& pos) {
	WorldTransform world(pos,XMFLOAT3(1.5f,1.9f,1.5f));
	ModelPtrS model(std::make_shared<Model>(L"Asset/Barrel.obj",L"Shader/Phong.hlsl"));
	GameObjectPtrS barreal(std::make_shared<GameObject>(model,std::move(world)));
	DynamicRigidPtr rigidBody(std::make_shared<DynamicRigidBody>(*barreal,0.1f));
	rigidBody->AttachBox(0.7f, 0.9f,0.7f, {1.0f,0.7f,0.0f},"Barrel");
	barreal->AddSharedComponent(rigidBody);
	std::shared_ptr<BarrelBehavior> behavior = std::make_shared<BarrelBehavior>(*barreal,mGameObjects[ObjType::Person][0],rigidBody);
	barreal->AddSharedComponent(behavior);
	AddGameObject(ObjType::Other, barreal);
}

//------------------------------------
//  ステータスUI作成
//------------------------------------
void Stage::CreateStatusUI() {
	// ステータスUIの作成
	WorldTransform world(XMFLOAT3(0.0f, 0.4f, 0.8f),XMFLOAT3(0.8f,0.8f,0.8f));
	ModelPtrS statusUI(std::make_shared<Model>(Shape::Square, L"Asset/StatusNew.jpg", L"Shader/ForUI.hlsl"));
	GameObjectPtrS status = std::make_shared<GameObject>(statusUI,std::move(world));
	AddGameObject(ObjType::Other,status);  // ハッシュに追加
}

//---------------------------------------
//  ブロック作成
//---------------------------------------
void Stage::CreateBlock(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale,const std::wstring& textureName) {
	// ブロック作成
	WorldTransform world(pos,scale);
	ModelPtrS blockModel = mModelManager->GetModel(textureName);
	GameObjectPtrS block = std::make_shared<GameObject>(blockModel,std::move(world));

	StaticRigidPtr physics = std::make_shared<StaticRigidBody>(*block);
	physics->AttachBox(scale.x,scale.y,scale.z);
	// 貫通対策のための当たり判定。ブロックの頭に少しワイドな判定を持たせる。
	physics->AttachBox(scale.x + 0.05f, 0.01f, scale.z + 0.05f, {},"Object",DirectX::XMFLOAT3(0.0f,scale.y,0.0f));
	// 貫通対策のためのトリガー判定。DynamicRigidBodyでAttachPreventingPenetrationBox()で貫通対策をしたオブジェクトはこの判定と当たると後退する
	physics->AttachBoxTrigger(scale.x * 1.1f, scale.y * 0.9f, scale.z * 1.1f, PhysicsMaterial{},"Object");
	block->AddSharedComponent(physics);
	AddGameObject(ObjType::Cube,block);  // ハッシュに追加
}


//--------------------------------------------
//  階段作成
//--------------------------------------------
void Stage::CreateStair(int numSteps, const DirectX::XMFLOAT3& firstStepPos) {
	for (int i = 0; i < numSteps; i++) {
		XMFLOAT3 pos(firstStepPos.x + (i*2.0f),firstStepPos.y-2.0f,firstStepPos.z);
		for (int j = 0; j <= i; j++) {
			pos.y = pos.y + 2.0f;
			CreateBlock(pos, XMFLOAT3(1.0f, 1.0f, 1.0f));
		}
	}
}

//--------------------------------------
//  タイムに応じたメダル作成
//--------------------------------------
void Stage::CreateMedal(DWORD clearTime) {
	JsonObject obj = mJsonFile->GetJsonObject();
	JsonObject stageInfo = obj.GetObjectData(mJsonStageObjectName.c_str());
	// タイムに応じてメダルのテクスチャ名を変える
	DWORD goldMedalTime = stageInfo.GetInt("GoldMedalTime");
	DWORD silverMedalTime = stageInfo.GetInt("SilverMedalTime");
	std::wstring medalTextureName;
	if (clearTime <= goldMedalTime) {
		medalTextureName = L"Asset/goldMedal.png";
		mMedal = Medal::EGold;
	}
	else if (clearTime <= silverMedalTime) {
		medalTextureName = L"Asset/silverMedal.png";
		mMedal = Medal::ESilver;
	}
	else {
		medalTextureName = L"Asset/bronzeMedal.png";
		mMedal = Medal::EBronze;
	}

	// メダル作成
	WorldTransform world(XMFLOAT3(0.0f,0.0f,0.2f),XMFLOAT3(0.5f,0.7f,0.5f));
	ModelPtrS  model = std::make_shared<Model>(Shape::Square,medalTextureName,L"Shader/ForUI.hlsl");
	GameObjectPtrS medal = std::make_shared<GameObject>(model, std::move(world));
	AddGameObject(ObjType::Other,medal); // ハッシュに追加
}


//---------------------------------
//   宝箱作成
//---------------------------------
void Stage::CreateTreasureBox(const XMFLOAT3& pos,const DirectX::XMVECTOR& quaternion) {
	WorldTransform world(pos,XMFLOAT3(1.5f,1.5f,1.5f), quaternion);
	ModelPtrS model = std::make_shared<Model>(L"Asset/treasure.obj",L"Shader/Phong.hlsl");
	GameObjectPtrS treasureBox = std::make_shared<GameObject>(model,std::move(world));
	StaticRigidPtr rigidBody = std::make_shared<StaticRigidBody>(*treasureBox);
	rigidBody->AttachBoxTrigger(0.7, 0.7f, 0.4f, {},"TreasureBox");
	rigidBody->SetTriggerFunc([this](const HitObjectInfo& info) {if (info.name == "Person") mIsClear = true; }, "TreasureBox");
	treasureBox->AddSharedComponent(rigidBody);
	AddGameObject(ObjType::Other,treasureBox);
}

//---------------------------------
//  リザルト画面作成
//---------------------------------
void Stage::CreateResult() {
	if (mWasCreatedResult) return;  // リザルトがすでにつくられてるなら帰る

	// リザルトを作る前に人とカエルはストップさせておく
	mGameObjects[ObjType::Person][0]->SetState(GameObject::State::EStop);
	mGameObjects[ObjType::Kaeru][0]->SetState(GameObject::State::EStop);
	mBGMSound->Stop(); // BGMも消す

	CreateMedal(mTimer->GetTime());
	mAudioSystem->PlayEvent("event:/clear2");  // クリア用SEを鳴らす
	DWORD clearTime = mTimer->GetTime();

	// クリアタイムで初期化されたタイマーを作ることで、クリアタイムを表示する
	mTimer.reset(new Timer(XMFLOAT3(0.1f, -0.65f, 0.0f), XMFLOAT3(0.15f, 0.15f, 0.15f), clearTime));
	mTimer->Stop();

	// Timeと書かれたテクスチャを作る
	WorldTransform world(XMFLOAT3(-0.35f,-0.6f,0.1f),XMFLOAT3(0.3f,0.3f,0.3f));
	ModelPtrS model = std::make_shared<Model>(Shape::Square,L"Asset/Time.png",L"Shader/ForUI.hlsl");
	GameObjectPtrS timeStr = std::make_shared<GameObject>(model,std::move(world));
	AddGameObject(ObjType::Other,timeStr);

	mWasCreatedResult = true; // リザルトは出来上がってます
}

//--------------------------------------------
//   クリアタイムとメダル情報のセーブ
//--------------------------------------------
void Stage::SaveCrearTimeAndMedal() {
	JsonObject object = mJsonFile->GetJsonObject();
	JsonObject stageInfo = object.GetObjectData(mJsonStageObjectName.c_str());
	int bestClearTime = stageInfo.GetInt("Time");  // 歴代最速タイムを取得
	int currentGameClearTime = mTimer->GetTime();  // 今回のクリアタイムを取得

	// ベストタイムを更新していたらセーブする
	if (currentGameClearTime <= bestClearTime && mIsClear) {
		stageInfo.SetInt("Time", currentGameClearTime);
		stageInfo.SetInt("Medal", mMedal);
		mJsonFile->SaveFile();
	}
}


void Stage::LoadModels() {
	// 再利用されるモデルはマネージャーに登録して、メモリを節約する。

	mModelManager->AddModel(Shape::Cube, L"Asset/CubeTex.png", L"Shader/NoLighting.hlsl");
	mModelManager->AddModel(Shape::Cube,L"Asset/Dirt.png",L"Shader/NoLighting.hlsl");
	mModelManager->AddModel(Shape::Cube,L"Asset/Wood.png",L"Shader/NoLighting.hlsl");
}