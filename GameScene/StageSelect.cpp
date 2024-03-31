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

// 例えばキーが「1」なら「Stage1作成関数」をペアに持つ 
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
	SetNumPlayableStage(); // プレイ可能ステージ数を設定する

	// ステージ名を指し示すarrowの回転やらスケールやら決める
	const DirectX::XMVECTOR arrowQuaternion = Quaternion::RotateEuler(0.0f,0.0f,90.0f);
	mArrow->SetQuaternion(arrowQuaternion);
	const DirectX::XMFLOAT3 arrowScale(0.06f,0.06f,0.06f);
	mArrow->SetScale(arrowScale);

	mAudioSystem->StopAllSound();             // 一応全ての音を止めたのち
	mAudioSystem->PlayEvent("event:/Title");  // タイトル用のBGMをならす

	// 再利用されるモデルはモデルマネージャにまとめておく
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
	// Enterが押されたら矢印がさしているステージへ
	if (Input::GetKeyState(VK_RETURN) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_A) == InputState::EPush) {
		next = mStageNumberToStageMap[mStageNumberArrowPointing](); // 矢印がさしているステージをnewする関数を呼び出す。
	}

	const bool downKeyPressed = Input::GetKeyState(VK_DOWN) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_DPAD_DOWN) == InputState::EPush;
	// 下ボタンが押されたら矢印を少し下へ。プレイ可能ステージよりも下はさせない
	if (downKeyPressed && mStageNumberArrowPointing < mNumPlayableStage) {
		mArrow->AddPos(DirectX::XMFLOAT3(0.0f,-0.25f,0.0f));
		mStageNumberArrowPointing++; // 矢印が指し示しているステージが一つ上がる
	}
	const bool upKeyPressed = Input::GetKeyState(VK_UP) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_DPAD_UP) == InputState::EPush;
	// 上ボタンが押されたら矢印を少し上へ。ステージ1より上は指せない
	if (upKeyPressed && mStageNumberArrowPointing > 1) {
		mArrow->AddPos(DirectX::XMFLOAT3(0.0f, 0.25f, 0.0f));
		mStageNumberArrowPointing--; // 矢印が指し示しているステージが一つ下がる
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
	
	// 全ステージクリアしてるなら
	if (mAreAllStagesCleared) {
		mCompleteUI->Render(*mCamera); // Completeの画像を描画する
	}
	Model::EndRender();
}

//----------------------------------------------------------
//  ステージ毎のメダル情報、ベストタイム情報をつくる
//----------------------------------------------------------
void StageSelect::CreateStagesInfo() {
	// クリアした最後のステージの次のステージまで情報を作る。
	for (float i = 1; i <= mNumPlayableStage; i++) {
		if (i > numStages) break;  // iがステージ数を超えていたら書く情報がないのでブレイク
		// 少し位置を下にずらしてメダル、タイム、ステージ名をステージ毎に生成
		CreateMedal(DirectX::XMFLOAT3(0.6f, 0.92f - (i * 0.26f), 0.1f), static_cast<int>(i));
		CreateClearTime(DirectX::XMFLOAT3(0.8f, 0.92f - (i * 0.26f), 0.1f), static_cast<int>(i));
		CreateStageName(DirectX::XMFLOAT3(-0.25f,0.75f - (i * 0.25f),0.1f),static_cast<int>(i));
	}
}

//-----------------------------------
//  メダル作成
//-----------------------------------
void StageSelect::CreateMedal(const DirectX::XMFLOAT3& pos,int stageNumber) {
	JsonObject stage1object = GetJsonStageInfo(stageNumber);
	int medalType = stage1object.GetInt("Medal");  // jsonからstageNumberのメダルの種類を取得
	WorldTransform medalWorld(pos);
	ModelPtrS model;
	// メダルの種類に応じてテクスチャ名を決める
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
		int medalType = stage1object.GetInt("Medal");  // jsonからstageNumberのメダルの種類を取得

		// メダルタイプゼロのステージ、つまり未クリアステージまでがプレイ可能ステージとする
		// 例えば3ステージ目でメダルタイプ0なら3ステージ目までが遊べるステージとする。
		if (medalType == 0) {
			mNumPlayableStage = i; 
			return;
		}
	}
	mNumPlayableStage = numStages; // ループ内で決まらないということは、全てのステージが遊べる
	mAreAllStagesCleared = true;   // ループ内で決まられないということは全ステージクリアしてるということ
}


JsonObject StageSelect::GetJsonStageInfo(int stageNumber) {
	JsonObject object = mStageInfoJson->GetJsonObject();
	std::string stageName = "Stage" + std::to_string(stageNumber);
	return object.GetObjectData(stageName.c_str());
}