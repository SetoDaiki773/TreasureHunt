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
	// ���[�h���Ԃ��o�ߎ��ԂɊ܂߂Ȃ����߂ɁA�Ō��new����
	mTimer = std::make_unique<Timer>(XMFLOAT3(-0.1f, 0.87f, 0.3f), XMFLOAT3(0.08f, 0.08f, 0.08f));
}

Stage::~Stage() {
	mGameObjects.clear();
}



//------------------------------------
//  �V�[���̍X�V
//------------------------------------
Scene* Stage::Update() {
	PhysXWorld::Update();  // �������E���X�V����
	DynamicRigidBody::ReflectAllPosOfPhysXWorld(); // PhysX�̏�̃I�u�W�F�N�g�̈ʒu���Q�[�����E�ɔ��f����
	StaticRigidBody::ReflectAllPosOfPhysXWorld();  // static������
	mAudioSystem->Update();
	mAudioSystem->SetListenerPos(mCamera->GetPos(),mCamera->GetLookPos());
	Input::UpdateCurrentKey();
	mCamera->Update();
	// �S�ẴI�u�W�F�N�g���X�V
	for (auto objs : mGameObjects) {
		for (auto obj : objs.second) {
			obj->Update();
		}
	}
	// Dead��Ԃ̃I�u�W�F�N�g������
	for (auto& gameObjects : mGameObjects) {
		std::erase_if(gameObjects.second, [](GameObjectPtrS gameObject) { return gameObject->GetState() == GameObject::State::EDead; });
	}
	

	Scene* next = this;  // ���̃V�[���͂Ƃ肠�������Ɠ������̂ɂ��Ă���
	// �N���A���Ă����烊�U���g���o���B180�b�o���Ă���StageSelect�ֈړ�����
	if (mIsClear) {
		CreateResult();
		mElapsedTimeAfterClear++;
		// �N���A���Ă���180�t���[���o�߂����玟�̃V�[����StageSelect�֕ύX
		if (mElapsedTimeAfterClear >= 180.0f) {
			SaveCrearTimeAndMedal();  // �Z�[�u�����Ă���
			next = new StageSelect(); // ���̃V�[���̓X�e�[�W�Z���N�g���
		}
	}

	// �v���C���[������������BGM���~���āAGameOverBGM���Ȃ炷�BGameOverBGM���~�܂�����A���X�^�[�g�B

	// �v���C���[������������
	if (mGameObjects[ObjType::Person][0]->GetPos().y <= -10.0f) {
		// GameOver�p�̃T�E���h���Ȃ��Ă��Ȃ�炷�B
		if (mGameOverSound == nullptr) {
			mGameOverSound = std::make_unique<SoundEvent>(mAudioSystem->PlayEvent("event:/GameOver"));
		}
		mGameObjects[ObjType::Person][0]->SetState(GameObject::State::EStopUndraw);
		mBGMSound->Stop();
		// GameOber�T�E���h���~�܂�����
		if (!mGameOverSound->IsValid()) {
			next = new Stage(std::move(mStageCreater), mJsonStageObjectName);  // �����V�[��������������B�܂胊�X�^�[�g
		}
	}


	// R����������X�e�[�W��Ԃ����Z�b�g����
	if (Input::GetKeyState('R') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_RIGHT_SHOULDER) == InputState::EPush) {
		next = new Stage(std::move(mStageCreater), mJsonStageObjectName); // �{���I�ɂ͍��Ɠ����X�e�[�W��new����
	}
	
	// Q����������X�e�[�W�Z���N�g��
	if (Input::GetKeyState('Q') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_LEFT_SHOULDER) == InputState::EPush) {
		next = new StageSelect(); // ���̃V�[���̓X�e�[�W�Z���N�g
	}

	DynamicRigidBody::ReflectAllPosToPhysXWorld();  // �I�u�W�F�N�g�̍X�V���I������̂ŃQ�[�����E�̈ʒu�𕨗����E�ɔ��f������
	StaticRigidBody::ReflectAllPosToPhysXWorld();   // static������
	

	return next;
}


//------------------------------
//  �V�[���`��
//------------------------------
void Stage::Render() {
	NumberUI redKeyCountUI(XMFLOAT3(0.35f, 0.87f, 0.3f), XMFLOAT3(0.08f, 0.08f, 0.08f));
	NumberUI blueKeyCountUI(XMFLOAT3(0.6f, 0.87f, 0.3f), XMFLOAT3(0.08f, 0.08f, 0.08f));
	// �S�I�u�W�F�N�g�ƃ^�C�}�[�A���̐���`��
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
//  �Q�[���I�u�W�F�N�g���n�b�V���ɒǉ�
//-----------------------------------------
void Stage::AddGameObject(ObjType objType, GameObjectPtrS obj) {
	mGameObjects[objType].push_back(obj);
}



//-----------------------------------
//  �l�쐬
//-----------------------------------
void Stage::CreatePerson(const XMFLOAT3& pos) {
	// ���f���ƃ��[���h�ϊ���񂩂�v���C���[�̌����쐬
	std::shared_ptr<Model> personModel = std::make_shared<Model>(L"Asset/boy.obj", L"Shader/Phong.hlsl", L"Shader/Phong.hlsl");
	WorldTransform world(pos,XMFLOAT3(1.5f,1.8f,1.5f));
	GameObjectPtrS person = std::make_shared<GameObject>(personModel, std::move(world));

	// �v���C���[�𒍎�����TPS�J�����̒ǉ�
	std::unique_ptr<CameraMovement> testCamera(new TPSCamera(person.get(),0.0f,180.0f));
	mCamera = std::make_unique<Camera>(XMFLOAT3(0.0f, 0.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), std::move(testCamera));

	// �����v�f�̒ǉ�
	std::shared_ptr<DynamicRigidBody> physics = std::make_shared<DynamicRigidBody>(*person,1.0f);
	physics->AttachBox(0.5f, 0.9f, 0.5f, {1.0f,1.0f,0.0f},"Person");
	physics->AttachBox(0.48f, 0.2f, 0.48f, {}, "PersonHead",XMFLOAT3(0.0f,0.8f,0.0f));
	physics->AttachBoxTrigger(0.48f, 0.3f, 0.48f, {},"Foot",XMFLOAT3(0.0f,-1.0f,0.0f));
	person->AddSharedComponent(physics);

	// �I�[�f�B�I�v�f�ǉ�
	std::shared_ptr<AudioComponent> audio = std::make_shared<AudioComponent>(*person);
	person->AddSharedComponent(audio);

	// ���͏����v�f�̒ǉ�
	std::shared_ptr<PlayerInputComponent> pInputComp(std::make_shared<PlayerInputComponent>(*person, mCamera.get(),audio.get(),physics.get(), 11.45f, true));
	person->AddSharedComponent(pInputComp);

	// �v���C���[�̍s���v�f�̒ǉ�
	std::shared_ptr<PlayerBehavior> behavior = std::make_shared<PlayerBehavior>(*person, physics,audio);
	person->AddSharedComponent(behavior);
	
	AddGameObject(ObjType::Person, person);  // �n�b�V���ɒǉ�
}


//-------------------------------------------
//   �J�G���쐬
//-------------------------------------------
void Stage::CreateKaeru(const XMFLOAT3& pos) {
	// �J�G���̌����쐬
	std::shared_ptr<Model> kaeruModel = std::make_shared<Model>(L"Asset/kaeru.obj", L"Shader/Phong.hlsl", L"Shader/Phong.hlsl");
	WorldTransform kaeruWorld(pos,XMFLOAT3(1.5f,1.5f,1.5f));
	GameObjectPtrS kaeru = std::make_shared<GameObject>(kaeruModel, std::move(kaeruWorld));

	// �����v�f�̒ǉ�
	std::shared_ptr<DynamicRigidBody> phy = std::make_shared<DynamicRigidBody>(*kaeru,0.1f);
	phy->AttachBox(0.6f, 0.6f, 0.6f, {},"Kaeru");
	phy->AttachBoxTrigger(0.3f, 0.3f, 0.3f, {}, "KaeruFoot", XMFLOAT3(0.0f, -0.6f, 0.0f));
	kaeru->AddSharedComponent(phy);

	// �I�[�f�B�I�v�f�ǉ�
	std::shared_ptr<AudioComponent> audio = std::make_shared<AudioComponent>(*kaeru);
	kaeru->AddSharedComponent(audio);

	// ���͏����v�f�̒ǉ�
	std::shared_ptr<PlayerInputComponent> kaeruInput(std::make_shared<PlayerInputComponent>(*kaeru, mCamera.get(),audio.get(),phy.get(), 14.5f, false));
	kaeru->AddSharedComponent(kaeruInput);

	// �J�G���̍s���v�f�̒ǉ�
	std::shared_ptr<KaeruBehavior> behavior = std::make_shared<KaeruBehavior>(*kaeru,phy,mGameObjects[ObjType::Person][0],kaeruInput,audio);
	kaeru->AddSharedComponent(behavior);
	AddGameObject(ObjType::Kaeru, kaeru); // �n�b�V���ɒǉ�
}


//------------------------------------
//  �X�J�C�}�b�v�쐻
//------------------------------------
void Stage::CreateSkyMap() {
	// sky�}�b�v�̍쐬
	std::shared_ptr<Model> skyModel = std::make_shared<Model>(L"Asset/Sky_Sphere_R10.obj", L"Shader/Phong.hlsl");
	XMVECTOR quaternion = Quaternion::RotateEuler(180.0f, 0.0f, 0.0f);
	WorldTransform world2(XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), quaternion);
	GameObjectPtrS sky = std::make_shared<GameObject>(skyModel, std::move(world2));
	AddGameObject(ObjType::Sky, sky);  // hash�ɒǉ�
}

//-------------------------------
//  ���쐬
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

		std::map<Color, unsigned*> colorToKeyCountMap; // �L�[�J���[���炻�̐F�̃J�M�̏��������A�N�Z�X���邽�߂̃}�b�v
		colorToKeyCountMap[Color::ERed] = &mKeyCounter->redKeyCount;
		colorToKeyCountMap[Color::EBlue] = &mKeyCounter->blueKeyCount;
		auto iter = colorToKeyCountMap.find(color); // �����̐F�Ɠ�������
		*(iter->second) = *(iter->second) + 1;       // ����₷
		keyP->SetState(GameObject::State::EDead);   // ���͎擾���ꂽ�̂Ŏ���
		mAudioSystem->PlayEvent("event:/GetKey");
	};
	rigidBody->SetTriggerFunc(addKeyCount,"Key");
	key->AddSharedComponent(rigidBody);
	AddGameObject(ObjType::Other, key);
}

//------------------------------------
//  ���b�N�쐬
//------------------------------------
void Stage::CreateLock(const XMFLOAT3& pos,Color color) {
	std::map <Color, std::wstring> colorToTextureName{
		{Color::ERed,L"Asset/RedLock.jpg"},
		{Color::EBlue,L"Asset/BlueLock.jpg"}
	};
	std::wstring textureName = colorToTextureName[color];  // map����e�N�X�`����������
	WorldTransform world(pos); 
	ModelPtrS model = std::make_shared<Model>(Shape::Cube,textureName,L"Shader/NoLighting.hlsl"); // Lock�̓L���[�u�^
	GameObjectPtrS lock = std::make_shared<GameObject>(model,std::move(world));

	// ���̗v�f
	StaticRigidPtr rigidBody = std::make_shared<StaticRigidBody>(*lock);
	rigidBody->AttachBox(1.0f, 1.0f, 1.0f, {});
	rigidBody->AttachBoxTrigger(1.1f, 1.1f, 1.1f, {},"LockTrigger");
	GameObject* lockP = lock.get();

	// ���b�N����������֐�
	auto releaseLock = [this,color,lockP](const HitObjectInfo& info) {
		if (info.triggerEvent == TriggerEvent::EOut) return;
		std::map<Color, unsigned*> colorToKeyCountMap; // �L�[�J���[���炻�̐F�̃J�M�̏��������A�N�Z�X���邽�߂̃}�b�v
		colorToKeyCountMap[Color::ERed] = &mKeyCounter->redKeyCount;
		colorToKeyCountMap[Color::EBlue] = &mKeyCounter->blueKeyCount;
		// �l�ɓ������āA���b�N�̐F�ɑΉ��������̐���0���傫���Ȃ�B
		if ((info.name == "Person") && *(colorToKeyCountMap[color]) > 0) {
			auto iter = colorToKeyCountMap.find(color);
			*(iter->second) = *(iter->second) - 1;      // ��������炵��
			lockP->SetState(GameObject::State::EDead);  // ���b�N����
			mAudioSystem->PlayEvent("event:/ReleaseLock");
		}
	};
	rigidBody->SetTriggerFunc(releaseLock,"LockTrigger");  // �g���K�[�Ɋ֐����Z�b�g
	lock->AddSharedComponent(rigidBody);

	AddGameObject(ObjType::Other,lock);
}

//------------------------------------
//  �M�쐬
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
//  �X�e�[�^�XUI�쐬
//------------------------------------
void Stage::CreateStatusUI() {
	// �X�e�[�^�XUI�̍쐬
	WorldTransform world(XMFLOAT3(0.0f, 0.4f, 0.8f),XMFLOAT3(0.8f,0.8f,0.8f));
	ModelPtrS statusUI(std::make_shared<Model>(Shape::Square, L"Asset/StatusNew.jpg", L"Shader/ForUI.hlsl"));
	GameObjectPtrS status = std::make_shared<GameObject>(statusUI,std::move(world));
	AddGameObject(ObjType::Other,status);  // �n�b�V���ɒǉ�
}

//---------------------------------------
//  �u���b�N�쐬
//---------------------------------------
void Stage::CreateBlock(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale,const std::wstring& textureName) {
	// �u���b�N�쐬
	WorldTransform world(pos,scale);
	ModelPtrS blockModel = mModelManager->GetModel(textureName);
	GameObjectPtrS block = std::make_shared<GameObject>(blockModel,std::move(world));

	StaticRigidPtr physics = std::make_shared<StaticRigidBody>(*block);
	physics->AttachBox(scale.x,scale.y,scale.z);
	// �ђʑ΍�̂��߂̓����蔻��B�u���b�N�̓��ɏ������C�h�Ȕ������������B
	physics->AttachBox(scale.x + 0.05f, 0.01f, scale.z + 0.05f, {},"Object",DirectX::XMFLOAT3(0.0f,scale.y,0.0f));
	// �ђʑ΍�̂��߂̃g���K�[����BDynamicRigidBody��AttachPreventingPenetrationBox()�Ŋђʑ΍�������I�u�W�F�N�g�͂��̔���Ɠ�����ƌ�ނ���
	physics->AttachBoxTrigger(scale.x * 1.1f, scale.y * 0.9f, scale.z * 1.1f, PhysicsMaterial{},"Object");
	block->AddSharedComponent(physics);
	AddGameObject(ObjType::Cube,block);  // �n�b�V���ɒǉ�
}


//--------------------------------------------
//  �K�i�쐬
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
//  �^�C���ɉ��������_���쐬
//--------------------------------------
void Stage::CreateMedal(DWORD clearTime) {
	JsonObject obj = mJsonFile->GetJsonObject();
	JsonObject stageInfo = obj.GetObjectData(mJsonStageObjectName.c_str());
	// �^�C���ɉ����ă��_���̃e�N�X�`������ς���
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

	// ���_���쐬
	WorldTransform world(XMFLOAT3(0.0f,0.0f,0.2f),XMFLOAT3(0.5f,0.7f,0.5f));
	ModelPtrS  model = std::make_shared<Model>(Shape::Square,medalTextureName,L"Shader/ForUI.hlsl");
	GameObjectPtrS medal = std::make_shared<GameObject>(model, std::move(world));
	AddGameObject(ObjType::Other,medal); // �n�b�V���ɒǉ�
}


//---------------------------------
//   �󔠍쐬
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
//  ���U���g��ʍ쐬
//---------------------------------
void Stage::CreateResult() {
	if (mWasCreatedResult) return;  // ���U���g�����łɂ����Ă�Ȃ�A��

	// ���U���g�����O�ɐl�ƃJ�G���̓X�g�b�v�����Ă���
	mGameObjects[ObjType::Person][0]->SetState(GameObject::State::EStop);
	mGameObjects[ObjType::Kaeru][0]->SetState(GameObject::State::EStop);
	mBGMSound->Stop(); // BGM������

	CreateMedal(mTimer->GetTime());
	mAudioSystem->PlayEvent("event:/clear2");  // �N���A�pSE��炷
	DWORD clearTime = mTimer->GetTime();

	// �N���A�^�C���ŏ��������ꂽ�^�C�}�[����邱�ƂŁA�N���A�^�C����\������
	mTimer.reset(new Timer(XMFLOAT3(0.1f, -0.65f, 0.0f), XMFLOAT3(0.15f, 0.15f, 0.15f), clearTime));
	mTimer->Stop();

	// Time�Ə����ꂽ�e�N�X�`�������
	WorldTransform world(XMFLOAT3(-0.35f,-0.6f,0.1f),XMFLOAT3(0.3f,0.3f,0.3f));
	ModelPtrS model = std::make_shared<Model>(Shape::Square,L"Asset/Time.png",L"Shader/ForUI.hlsl");
	GameObjectPtrS timeStr = std::make_shared<GameObject>(model,std::move(world));
	AddGameObject(ObjType::Other,timeStr);

	mWasCreatedResult = true; // ���U���g�͏o���オ���Ă܂�
}

//--------------------------------------------
//   �N���A�^�C���ƃ��_�����̃Z�[�u
//--------------------------------------------
void Stage::SaveCrearTimeAndMedal() {
	JsonObject object = mJsonFile->GetJsonObject();
	JsonObject stageInfo = object.GetObjectData(mJsonStageObjectName.c_str());
	int bestClearTime = stageInfo.GetInt("Time");  // ���ő��^�C�����擾
	int currentGameClearTime = mTimer->GetTime();  // ����̃N���A�^�C�����擾

	// �x�X�g�^�C�����X�V���Ă�����Z�[�u����
	if (currentGameClearTime <= bestClearTime && mIsClear) {
		stageInfo.SetInt("Time", currentGameClearTime);
		stageInfo.SetInt("Medal", mMedal);
		mJsonFile->SaveFile();
	}
}


void Stage::LoadModels() {
	// �ė��p����郂�f���̓}�l�[�W���[�ɓo�^���āA��������ߖ񂷂�B

	mModelManager->AddModel(Shape::Cube, L"Asset/CubeTex.png", L"Shader/NoLighting.hlsl");
	mModelManager->AddModel(Shape::Cube,L"Asset/Dirt.png",L"Shader/NoLighting.hlsl");
	mModelManager->AddModel(Shape::Cube,L"Asset/Wood.png",L"Shader/NoLighting.hlsl");
}