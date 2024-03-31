#include "Title.h"
#include "Input/Input.h"
#include "ObjectComponent/Model.h"
#include "Camera/StaticCamera.h"
#include "GameObject/GameObject.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/RigidBody.h"
#include "Camera/StaticCamera.h"
#include "Camera/Camera.h"
#include "Common/MyMath.h"
#include "Scene/Sequence.h"
#include "Common/AudioSystem.h"
#include "Scene/StageSelect.h"
#include "Common/FileUtil.h"

using namespace DirectX;

Title::Title()
	:mIsDisplayingHowToPlay(false)
	,mIsDisplayingPupose(false)
	,mIsDisplayingTitleOnly(true)
	,mHowToPlayPageIndex(0)
	,mPurposePageIndex(0)
	,mTitle(
		std::make_unique<GameObject>(std::make_shared<Model>(Shape::Square, L"Asset/Title.jpg", L"Shader/ForUI.hlsl"),
		WorldTransform(XMFLOAT3(DirectX::XMFLOAT3(0.0f,0.0f,0.9f)),XMFLOAT3(1.2f,1.3f,1.2f))))
	,mCamera(new Camera(DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),std::make_unique<StaticCamera>()))
{
	mAudioSystem->PlayEvent("event:/Title");
	CreateHowToPlayPages();
	CreatePurposePages();
}


Scene* Title::Update() {
	Input::UpdateCurrentKey();
	mAudioSystem->Update();
	Scene* next = this;

	const bool pushedStageSelectKey = Input::GetKeyState('A') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_A) == InputState::EPush;
	const bool gotoStageSelect = pushedStageSelectKey && mIsDisplayingTitleOnly;
	// Aが押されてかつタイトル表示のみなら、ステージセレクトへ移動
	if (gotoStageSelect) {
		next = new StageSelect{}; // 次のシーンをステージセレクトへ
	}
	const bool pushedDisplayHowToPlayKey = Input::GetKeyState('B') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_B) == InputState::EPush;
	const bool shouldDisplayHowToPlay = pushedDisplayHowToPlayKey && mIsDisplayingTitleOnly;
	// Bが押されてかつタイトル表示のみなら(操作方法も目的説明も表示されていなかったら)操作方法画面表示
	if (shouldDisplayHowToPlay) {
		mHowToPlayPageIndex++; // ページインデックスを0から1に
		mIsDisplayingHowToPlay = true;
		mIsDisplayingTitleOnly = false;
	}
	const bool pushedDisplayPurposeKey = Input::GetKeyState('Y') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_Y) == InputState::EPush;
	const bool shouldDisplayPurpose = pushedDisplayPurposeKey && mIsDisplayingTitleOnly;
	// Yが押されてかつタイトル表示のみなら目的画面を表示
	if (shouldDisplayPurpose) {
		mPurposePageIndex++; // ページインデックスを0から1に
		mIsDisplayingPupose = true;
		mIsDisplayingTitleOnly = false;
	}


	const bool gotoNextPage = Input::GetKeyState(VK_RBUTTON) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_RIGHT_SHOULDER) == InputState::EPush;
	// 右クリックでページ数を進める
	if (gotoNextPage) {
		// 操作方法が表示中なら、操作方法のページ数を進める
		if (mIsDisplayingHowToPlay) {
			mHowToPlayPageIndex++;
			// 最終ページだったらページを0に戻して、表示なし状態へ
			if (mHowToPlayPageIndex >= mHowToPlayPages.size()) {
				mHowToPlayPageIndex = 0;
				mIsDisplayingHowToPlay = false;
				mIsDisplayingTitleOnly = true;
			}
		}
		// 目的説明表示中なら、ページ数を進める
		if (mIsDisplayingPupose) {
			mPurposePageIndex++;
			// 最終ページだったらページ数を0に戻して、表示なし状態へ
			if (mPurposePageIndex >= mPurposePages.size()) {
				mPurposePageIndex = 0;
				mIsDisplayingPupose = false;
				mIsDisplayingTitleOnly = true;
			}
		}
	}
	return next;
}

void Title::Render() {
	Model::BeginRender();
	mTitle->Render(*mCamera);
	mHowToPlayPages[mHowToPlayPageIndex].Render(*mCamera); // 操作方法表示、pageIndexが0の時は何も表示されない
	mPurposePages[mPurposePageIndex].Render(*mCamera);     // 目的表示,pageIndexが0の時は何も表示されない
	Model::EndRender();
}

//------------------------------------
//  操作方法のページをつくる
//------------------------------------
void Title::CreateHowToPlayPages() {
	constexpr int numHowToPlayPages = 4;
	mHowToPlayPages.reserve(numHowToPlayPages + 1);

	// 0ページ目をつくる。0ページ目は透明なテクスチャにしておく
	ModelPtrS transparent = std::make_shared<Model>(Shape::Square,L"Asset/Nothing.png", L"Shader/ForUI.hlsl");
	GameObject howToPlayNoting(transparent, WorldTransform{});
	mHowToPlayPages.push_back(std::move(howToPlayNoting));

	// 1ページ目以降をつくる
	for (int pageCount = 1; pageCount <= numHowToPlayPages;pageCount++) {
		std::string textureName = "Asset/HowToPlay/HowToPlayPage" + std::to_string(pageCount) + ".jpg";
		ModelPtrS model = std::make_shared<Model>(Shape::Square,ToWideString(textureName),L"Shader/ForUINoDiscard.hlsl");
		WorldTransform world(DirectX::XMFLOAT3(0.0f,0.0f,0.1f),DirectX::XMFLOAT3(0.5f,0.8f,1.0f));
		GameObject howToPlayPage(model,std::move(world));
		mHowToPlayPages.push_back(std::move(howToPlayPage));
	}
	
}

//-----------------------------------
//  目的説明のページを作る
//-----------------------------------
void Title::CreatePurposePages() {
	constexpr int numPurposePages = 2;
	mPurposePages.reserve(numPurposePages);

	// 0ページ目を作る。0ページ目は透明なテクスチャにしておく
	ModelPtrS transparent = std::make_shared<Model>(Shape::Square,L"Asset/Nothing.png", L"Shader/ForUI.hlsl");
	GameObject purposeNoting(transparent, WorldTransform{});
	mPurposePages.push_back(std::move(purposeNoting));

	// 1ページ目以降を作る。
	for (int pageCount = 1; pageCount <= numPurposePages; pageCount++) {
		std::string textureName = "Asset/Purpose/PurposePage" + std::to_string(pageCount) + ".jpg";
		ModelPtrS model = std::make_shared<Model>(Shape::Square, ToWideString(textureName), L"Shader/ForUINoDiscard.hlsl");
		WorldTransform world(DirectX::XMFLOAT3{},DirectX::XMFLOAT3(0.5f,0.8f,0.8f));
		GameObject purposePage(model, std::move(world));
		mPurposePages.push_back(std::move(purposePage));
	}
}