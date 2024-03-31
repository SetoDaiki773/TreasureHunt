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
/// [�@�\] �Q�[���V�[���̈�B�X�e�[�W�Z���N�g��ʂ̍X�V�`�������B
//=======================================================================
class StageSelect : public Scene {
public:
	StageSelect();
	~StageSelect();

private:
	Scene* Update()override;
	void Render()override;

	void CreateStagesInfo();  // �X�e�[�W���Ƀx�X�g�N���A�^�C����擾���_����\������
	void CreateMedal(const DirectX::XMFLOAT3& pos,int stageNumber);
	void CreateClearTime(const DirectX::XMFLOAT3& pos,int stageNumber);
	void CreateStageName(const DirectX::XMFLOAT3& pos,int stageNumber);
	void SetNumPlayableStage();
	class JsonObject GetJsonStageInfo(int stageNumber);

	int mNumPlayableStage; // �v���C�\�ȃX�e�[�W��
	bool mAreAllStagesCleared;
	std::shared_ptr<class GameObject> mStageSelect;
	std::unique_ptr<class GameObject> mCompleteUI;
	std::unique_ptr<class GameObject> mArrow;
	std::vector<std::shared_ptr<class GameObject>> mMedals; // �X�e�[�W���̍ō����_��
	std::vector<class NumberUI> mClearTimes;  // �X�e�[�W���̍ō��N���A�^�C��
	std::vector<std::shared_ptr<class GameObject>> mStageNames; // �X�e�[�W����UI
	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class ModelManager> mModelManager;
	std::unique_ptr<class JsonFile> mStageInfoJson;
	static std::map<int, CreateStageFunc> mStageNumberToStageMap;  // �X�e�[�W�i���o�[���X�e�[�W�C���X�^���X�ɕϊ�����}�b�v
	int mStageNumberArrowPointing; // ��󂪎w�������Ă���X�e�[�W
};