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
	// A��������Ă��^�C�g���\���݂̂Ȃ�A�X�e�[�W�Z���N�g�ֈړ�
	if (gotoStageSelect) {
		next = new StageSelect{}; // ���̃V�[�����X�e�[�W�Z���N�g��
	}
	const bool pushedDisplayHowToPlayKey = Input::GetKeyState('B') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_B) == InputState::EPush;
	const bool shouldDisplayHowToPlay = pushedDisplayHowToPlayKey && mIsDisplayingTitleOnly;
	// B��������Ă��^�C�g���\���݂̂Ȃ�(������@���ړI�������\������Ă��Ȃ�������)������@��ʕ\��
	if (shouldDisplayHowToPlay) {
		mHowToPlayPageIndex++; // �y�[�W�C���f�b�N�X��0����1��
		mIsDisplayingHowToPlay = true;
		mIsDisplayingTitleOnly = false;
	}
	const bool pushedDisplayPurposeKey = Input::GetKeyState('Y') == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_Y) == InputState::EPush;
	const bool shouldDisplayPurpose = pushedDisplayPurposeKey && mIsDisplayingTitleOnly;
	// Y��������Ă��^�C�g���\���݂̂Ȃ�ړI��ʂ�\��
	if (shouldDisplayPurpose) {
		mPurposePageIndex++; // �y�[�W�C���f�b�N�X��0����1��
		mIsDisplayingPupose = true;
		mIsDisplayingTitleOnly = false;
	}


	const bool gotoNextPage = Input::GetKeyState(VK_RBUTTON) == InputState::EPush || Input::GetButtonState(XINPUT_GAMEPAD_RIGHT_SHOULDER) == InputState::EPush;
	// �E�N���b�N�Ńy�[�W����i�߂�
	if (gotoNextPage) {
		// ������@���\�����Ȃ�A������@�̃y�[�W����i�߂�
		if (mIsDisplayingHowToPlay) {
			mHowToPlayPageIndex++;
			// �ŏI�y�[�W��������y�[�W��0�ɖ߂��āA�\���Ȃ���Ԃ�
			if (mHowToPlayPageIndex >= mHowToPlayPages.size()) {
				mHowToPlayPageIndex = 0;
				mIsDisplayingHowToPlay = false;
				mIsDisplayingTitleOnly = true;
			}
		}
		// �ړI�����\�����Ȃ�A�y�[�W����i�߂�
		if (mIsDisplayingPupose) {
			mPurposePageIndex++;
			// �ŏI�y�[�W��������y�[�W����0�ɖ߂��āA�\���Ȃ���Ԃ�
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
	mHowToPlayPages[mHowToPlayPageIndex].Render(*mCamera); // ������@�\���ApageIndex��0�̎��͉����\������Ȃ�
	mPurposePages[mPurposePageIndex].Render(*mCamera);     // �ړI�\��,pageIndex��0�̎��͉����\������Ȃ�
	Model::EndRender();
}

//------------------------------------
//  ������@�̃y�[�W������
//------------------------------------
void Title::CreateHowToPlayPages() {
	constexpr int numHowToPlayPages = 4;
	mHowToPlayPages.reserve(numHowToPlayPages + 1);

	// 0�y�[�W�ڂ�����B0�y�[�W�ڂ͓����ȃe�N�X�`���ɂ��Ă���
	ModelPtrS transparent = std::make_shared<Model>(Shape::Square,L"Asset/Nothing.png", L"Shader/ForUI.hlsl");
	GameObject howToPlayNoting(transparent, WorldTransform{});
	mHowToPlayPages.push_back(std::move(howToPlayNoting));

	// 1�y�[�W�ڈȍ~������
	for (int pageCount = 1; pageCount <= numHowToPlayPages;pageCount++) {
		std::string textureName = "Asset/HowToPlay/HowToPlayPage" + std::to_string(pageCount) + ".jpg";
		ModelPtrS model = std::make_shared<Model>(Shape::Square,ToWideString(textureName),L"Shader/ForUINoDiscard.hlsl");
		WorldTransform world(DirectX::XMFLOAT3(0.0f,0.0f,0.1f),DirectX::XMFLOAT3(0.5f,0.8f,1.0f));
		GameObject howToPlayPage(model,std::move(world));
		mHowToPlayPages.push_back(std::move(howToPlayPage));
	}
	
}

//-----------------------------------
//  �ړI�����̃y�[�W�����
//-----------------------------------
void Title::CreatePurposePages() {
	constexpr int numPurposePages = 2;
	mPurposePages.reserve(numPurposePages);

	// 0�y�[�W�ڂ����B0�y�[�W�ڂ͓����ȃe�N�X�`���ɂ��Ă���
	ModelPtrS transparent = std::make_shared<Model>(Shape::Square,L"Asset/Nothing.png", L"Shader/ForUI.hlsl");
	GameObject purposeNoting(transparent, WorldTransform{});
	mPurposePages.push_back(std::move(purposeNoting));

	// 1�y�[�W�ڈȍ~�����B
	for (int pageCount = 1; pageCount <= numPurposePages; pageCount++) {
		std::string textureName = "Asset/Purpose/PurposePage" + std::to_string(pageCount) + ".jpg";
		ModelPtrS model = std::make_shared<Model>(Shape::Square, ToWideString(textureName), L"Shader/ForUINoDiscard.hlsl");
		WorldTransform world(DirectX::XMFLOAT3{},DirectX::XMFLOAT3(0.5f,0.8f,0.8f));
		GameObject purposePage(model, std::move(world));
		mPurposePages.push_back(std::move(purposePage));
	}
}