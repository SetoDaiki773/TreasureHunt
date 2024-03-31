#include "StageSelect.h"
#include "ObjectComponent/Model.h"
#include "Common/AudioSystem.h"
#include "Common/QuaternionOperation.h"
#include "GameObject/GameObject.h"
#include "ObjectComponent/WorldTransform.h"
#include "Camera/Camera.h"
#include "Camera/StaticCamera.h"
#include "Input/Input.h"
#include "Sequence.h"
#include "Stage.h"
#include "Json/JsonFile.h"
#include "Manager/ModelManager.h"
#include "Common/FileUtil.h"
#include "Number.h"
#include "StageCreater.h"

// �Ⴆ�΃L�[���u1�v�Ȃ�uStage1�쐬�֐��v���y�A�Ɏ��� 
std::map<int, CreateStageFunc> StageSelect::mStageNumberToStageMap =
{
	{1,[]() {return new Stage(std::make_unique<StageCreater>(),  "Stage1"); }},
	{2,[]() {return new Stage(std::make_unique<Stage2Creater>(), "Stage2"); }},
	{3,[]() {return new Stage(std::make_unique<Stage3Creater>(), "Stage3"); }},
	{4,[]() {return new Stage(std::make_unique<Stage4Creater>(), "Stage4"); }}
};

constexpr int numStages = 4;

StageSelect::StageSelect() 
	:mStageSelect(std::make_shared<GameObject>(
		std::make_shared<Model>(Shape::Square,L"Asset/StageSelect/StageSelectBack.jpg",L"Shader/ForUI.hlsl"),
		WorldTransform(DirectX::XMFLOAT3{0.0f,0.0f,0.2f}))
	)
	,mArrow(std::make_unique<GameObject>(
		std::make_shared<Model>(Shape::Square,L"Asset/Arrow.png",L"Shader/ForUI.hlsl"),
		WorldTransform(DirectX::XMFLOAT3(-0.85f,0.68f,0.1f)))
	)
	,mCompleteUI(std::make_unique<GameObject>(
		std::make_shared<Model>(Shape::Square,L"Asset/StageSelect/Complete.jpg",L"Shader/ForUI.hlsl"),
		WorldTransform(DirectX::XMFLOAT3(0.5f,-0.7f,0.0f),DirectX::XMFLOAT3(0.4f,0.4f,1.0f)))
	)
	,mCamera(std::make_unique<Camera>(DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), std::make_unique<StaticCamera>()))
	,mStageInfoJson(std::make_unique<JsonFile>("Json/StageInfo.json"))
	,mModelManager(std::make_unique<ModelManager>())
	,mStageNumberArrowPointing(1)
	,mNumPlayableStage(1)
	,mAreAllStagesCleared(false)
{
	SetNumPlayableStage(); // �v���C�\�X�e�[�W����ݒ肷��

	// �X�e�[�W�����w������arrow�̉�]���X�P�[����猈�߂�
	const DirectX::XMVECTOR arrowQuaternion = Quaternion::RotateEuler(0.0f,0.0f,90.0f);
	mArrow->SetQuaternion(arrowQuaternion);
	const DirectX::XMFLOAT3 arrowScale(0.06f,0.06f,0.06f);
	mArrow->SetScale(arrowScale);

	mAudioSystem->StopAllSound();             // �ꉞ�S�Ẳ����~�߂��̂�
	mAudioSystem->PlayEvent("event:/Title");  // �^�C�g���p��BGM���Ȃ炷

	// �ė��p����郂�f���̓��f���}�l�[�W���ɂ܂Ƃ߂Ă���
	mModelManager->AddModel(Shape::Square, L"Asset/bronzeMedal.png", L"Shader/ForUI.hlsl");
	mModelManager->AddModel(Shape::Square, L"Asset/silverMedal.png", L"Shader/ForUI.hlsl");
	mModelManager->AddModel(Shape::Square, L"Asset/goldMedal.png", L"Shader/ForUI.hlsl");
	mModelManager->AddModel(Shape::Square, L"Asset/Nothing.png" , L"Shader/ForUI.hlsl");
	
	CreateStagesInfo();
}

StageSelect::~StageSelect() {
	
}



Scene* StageSelect::Update() {
	Input::UpdateCurrentKey();
	mAudioSystem->Update();
	Scene* next = this;
	// Enter�������ꂽ���󂪂����Ă���X�e�[�W��
	if (Input::GetKeyState(VK_RETURN) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_A) == InputState::EPush) {
		next = mStageNumberToStageMap[mStageNumberArrowPointing](); // ��󂪂����Ă���X�e�[�W��new����֐����Ăяo���B
	}

	const bool downKeyPressed = Input::GetKeyState(VK_DOWN) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_DPAD_DOWN) == InputState::EPush;
	// ���{�^���������ꂽ������������ցB�v���C�\�X�e�[�W�������͂����Ȃ�
	if (downKeyPressed && mStageNumberArrowPointing < mNumPlayableStage) {
		mArrow->AddPos(DirectX::XMFLOAT3(0.0f,-0.25f,0.0f));
		mStageNumberArrowPointing++; // ��󂪎w�������Ă���X�e�[�W����オ��
	}
	const bool upKeyPressed = Input::GetKeyState(VK_UP) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_DPAD_UP) == InputState::EPush;
	// ��{�^���������ꂽ�����������ցB�X�e�[�W1����͎w���Ȃ�
	if (upKeyPressed && mStageNumberArrowPointing > 1) {
		mArrow->AddPos(DirectX::XMFLOAT3(0.0f, 0.25f, 0.0f));
		mStageNumberArrowPointing--; // ��󂪎w�������Ă���X�e�[�W���������
	}
	return next;
}

void StageSelect::Render() {
	Model::BeginRender();
	mStageSelect->Render(*mCamera);
	mArrow->Render(*mCamera);
	
	for (auto medal :mMedals) {
		medal->Render(*mCamera);
	}
	for (auto& time : mClearTimes) {
		time.Render(*mCamera);
	}
	for (auto stageName : mStageNames) {
		stageName->Render(*mCamera);
	}
	
	// �S�X�e�[�W�N���A���Ă�Ȃ�
	if (mAreAllStagesCleared) {
		mCompleteUI->Render(*mCamera); // Complete�̉摜��`�悷��
	}
	Model::EndRender();
}

//----------------------------------------------------------
//  �X�e�[�W���̃��_�����A�x�X�g�^�C����������
//----------------------------------------------------------
void StageSelect::CreateStagesInfo() {
	// �N���A�����Ō�̃X�e�[�W�̎��̃X�e�[�W�܂ŏ������B
	for (float i = 1; i <= mNumPlayableStage; i++) {
		if (i > numStages) break;  // i���X�e�[�W���𒴂��Ă����珑����񂪂Ȃ��̂Ńu���C�N
		// �����ʒu�����ɂ��炵�ă��_���A�^�C���A�X�e�[�W�����X�e�[�W���ɐ���
		CreateMedal(DirectX::XMFLOAT3(0.6f, 0.92f - (i * 0.26f), 0.1f), static_cast<int>(i));
		CreateClearTime(DirectX::XMFLOAT3(0.8f, 0.92f - (i * 0.26f), 0.1f), static_cast<int>(i));
		CreateStageName(DirectX::XMFLOAT3(-0.25f,0.75f - (i * 0.25f),0.1f),static_cast<int>(i));
	}
}

//-----------------------------------
//  ���_���쐬
//-----------------------------------
void StageSelect::CreateMedal(const DirectX::XMFLOAT3& pos,int stageNumber) {
	JsonObject stage1object = GetJsonStageInfo(stageNumber);
	int medalType = stage1object.GetInt("Medal");  // json����stageNumber�̃��_���̎�ނ��擾
	WorldTransform medalWorld(pos);
	ModelPtrS model;
	// ���_���̎�ނɉ����ăe�N�X�`���������߂�
	switch(medalType) {
	case Medal::EBronze:
		model = mModelManager->GetModel(L"Asset/bronzeMedal.png");
		medalWorld.SetScale(DirectX::XMFLOAT3(0.04f,0.07f,0.05f));
		break;
	case Medal::ESilver:
		model = mModelManager->GetModel(L"Asset/silverMedal.png");
		medalWorld.SetScale(DirectX::XMFLOAT3(0.1f,0.1f,0.1f));
		break;
	case Medal::EGold: 
		model = mModelManager->GetModel(L"Asset/goldMedal.png");
		medalWorld.SetScale(DirectX::XMFLOAT3(0.05f,0.07f,0.05f));
		break;
	default:
		model = mModelManager->GetModel(L"Asset/Nothing.png");
		break;
	}
	GameObjectPtrS medal = std::make_shared<GameObject>(model, std::move(medalWorld));
	mMedals.push_back(medal);
}

void StageSelect::CreateClearTime(const DirectX::XMFLOAT3& pos,int stageNumber) {
	JsonObject stage1object = GetJsonStageInfo(stageNumber);
	int clearTime = stage1object.GetInt("Time");
	NumberUI number(pos,DirectX::XMFLOAT3(0.07f,0.07f,0.07f),clearTime);
	mClearTimes.push_back(std::move(number));
}

void StageSelect::CreateStageName(const DirectX::XMFLOAT3& pos,int stageNumber) {
	std::string stage = "Stage";
	stage = stage + std::to_string(stageNumber) + ".jpg";

	ModelPtrS model = std::make_shared<Model>(Shape::Square,L"Asset/StageSelect/" + ToWideString(stage),L"Shader/ForUI.hlsl");
	GameObjectPtrS stageNameUI = std::make_shared<GameObject>(model,WorldTransform(pos,DirectX::XMFLOAT3(0.8f,0.8f,0.8f)));
	mStageNames.push_back(stageNameUI);
}


void StageSelect::SetNumPlayableStage() {
	for (int i = 1; i <= numStages;i++) {
		JsonObject stage1object = GetJsonStageInfo(i);
		int medalType = stage1object.GetInt("Medal");  // json����stageNumber�̃��_���̎�ނ��擾

		// ���_���^�C�v�[���̃X�e�[�W�A�܂薢�N���A�X�e�[�W�܂ł��v���C�\�X�e�[�W�Ƃ���
		// �Ⴆ��3�X�e�[�W�ڂŃ��_���^�C�v0�Ȃ�3�X�e�[�W�ڂ܂ł��V�ׂ�X�e�[�W�Ƃ���B
		if (medalType == 0) {
			mNumPlayableStage = i; 
			return;
		}
	}
	mNumPlayableStage = numStages; // ���[�v���Ō��܂�Ȃ��Ƃ������Ƃ́A�S�ẴX�e�[�W���V�ׂ�
	mAreAllStagesCleared = true;   // ���[�v���Ō��܂��Ȃ��Ƃ������Ƃ͑S�X�e�[�W�N���A���Ă�Ƃ�������
}


JsonObject StageSelect::GetJsonStageInfo(int stageNumber) {
	JsonObject object = mStageInfoJson->GetJsonObject();
	std::string stageName = "Stage" + std::to_string(stageNumber);
	return object.GetObjectData(stageName.c_str());
}