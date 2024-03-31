#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <DirectXMath.h>

using CreateStageFunc = std::function<Scene* ()>;

//=======================================================================
/// [機能] ゲームシーンの一つ。ステージセレクト画面の更新描画をする。
//=======================================================================
class StageSelect : public Scene {
public:
	StageSelect();
	~StageSelect();

private:
	Scene* Update()override;
	void Render()override;

	void CreateStagesInfo();  // ステージ毎にベストクリアタイムや取得メダルを表示する
	void CreateMedal(const DirectX::XMFLOAT3& pos,int stageNumber);
	void CreateClearTime(const DirectX::XMFLOAT3& pos,int stageNumber);
	void CreateStageName(const DirectX::XMFLOAT3& pos,int stageNumber);
	void SetNumPlayableStage();
	class JsonObject GetJsonStageInfo(int stageNumber);

	int mNumPlayableStage; // プレイ可能なステージ数
	bool mAreAllStagesCleared;
	std::shared_ptr<class GameObject> mStageSelect;
	std::unique_ptr<class GameObject> mCompleteUI;
	std::unique_ptr<class GameObject> mArrow;
	std::vector<std::shared_ptr<class GameObject>> mMedals; // ステージ毎の最高メダル
	std::vector<class NumberUI> mClearTimes;  // ステージ毎の最高クリアタイム
	std::vector<std::shared_ptr<class GameObject>> mStageNames; // ステージ名のUI
	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class ModelManager> mModelManager;
	std::unique_ptr<class JsonFile> mStageInfoJson;
	static std::map<int, CreateStageFunc> mStageNumberToStageMap;  // ステージナンバーをステージインスタンスに変換するマップ
	int mStageNumberArrowPointing; // 矢印が指し示しているステージ
};