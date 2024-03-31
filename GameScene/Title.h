#pragma once
#include "Scene.h"
#include <memory>
#include <vector>

//===================================================================
/// [機能] ゲームシーンの一つ。タイトル画面全体を更新、描画する。
//===================================================================
class Title : public Scene {
public:
	Title();
	

private:
	Scene* Update()override;
	void Render()override;
	void CreateHowToPlayPages();
	void CreatePurposePages();

	bool mIsDisplayingHowToPlay;  // 操作方法表示中
	bool mIsDisplayingPupose;     // 目的説明画面表示中
	bool mIsDisplayingTitleOnly;  // タイトルだけ表示中
	int mHowToPlayPageIndex;      // 操作方法の現在ページ
	int mPurposePageIndex;        // 目的説明の現在ページ
	std::unique_ptr<class GameObject> mTitle;
	std::unique_ptr<class Camera> mCamera;
	std::vector<class GameObject> mHowToPlayPages;
	std::vector<class GameObject> mPurposePages;
};