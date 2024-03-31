#pragma once
#include "Scene.h"
#include <memory>
#include <vector>

//===================================================================
/// [�@�\] �Q�[���V�[���̈�B�^�C�g����ʑS�̂��X�V�A�`�悷��B
//===================================================================
class Title : public Scene {
public:
	Title();
	

private:
	Scene* Update()override;
	void Render()override;
	void CreateHowToPlayPages();
	void CreatePurposePages();

	bool mIsDisplayingHowToPlay;  // ������@�\����
	bool mIsDisplayingPupose;     // �ړI������ʕ\����
	bool mIsDisplayingTitleOnly;  // �^�C�g�������\����
	int mHowToPlayPageIndex;      // ������@�̌��݃y�[�W
	int mPurposePageIndex;        // �ړI�����̌��݃y�[�W
	std::unique_ptr<class GameObject> mTitle;
	std::unique_ptr<class Camera> mCamera;
	std::vector<class GameObject> mHowToPlayPages;
	std::vector<class GameObject> mPurposePages;
};