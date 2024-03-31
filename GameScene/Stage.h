#pragma once
#include "Scene.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <unordered_map>
#include <windows.h>
#include <string>

using GameObjects = std::unordered_map<enum class ObjType, std::vector<std::shared_ptr<class GameObject>>>;

// Json�ɐ��l�ő�����邽�߂ɂ�����enum class �ɂ��Ȃ�
enum Medal {
	ENothing,
	EBronze,
	ESilver,
	EGold
};


//==========================================================================================================================
/// [�@�\] �Q�[���V�[���̈�B�X�e�[�W�̍X�V��`�������B�X�e�[�W���\�z����̂�StageCreater�N���X�B
//==========================================================================================================================
class Stage :public Scene{
public:

	Stage(std::unique_ptr<class StageCreater> stageCreater,const std::string& jsonStageObjectName);
	~Stage();
	
	void LoadModels();

	
	//---------------------------------
	// [�@�\] �I�u�W�F�N�g�𐶐�����
	//---------------------------------
	void CreateSkyMap();
	void CreatePerson(const DirectX::XMFLOAT3& pos);
	void CreateKaeru(const DirectX::XMFLOAT3& pos);
	void CreateStatusUI();
	void CreateBlock(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1.0f,1.0f,1.0f),const std::wstring& textureName = L"Asset/CubeTex.png");
	void CreateStair(int numSteps, const DirectX::XMFLOAT3& firstStepPos);
	void CreateTreasureBox(const DirectX::XMFLOAT3& pos,const DirectX::XMVECTOR& quat = DirectX::XMQuaternionRotationRollPitchYaw(0.0f,3.14f,0.0f));
	void CreateKey(const DirectX::XMFLOAT3& pos,enum class Color keyColor);
	void CreateLock(const DirectX::XMFLOAT3& pos,enum class Color lockColor);
	void CreateBarrel(const DirectX::XMFLOAT3& pos);

private:
	void AddGameObject(enum class ObjType, std::shared_ptr<class GameObject> obj);


	
	Scene* Update()override;
	void Render()override;
	void CreateMedal(DWORD clearTime);
	void CreateResult();           // ���U���g��ʂ����
	void SaveCrearTimeAndMedal();  // �N���A�^�C���Ǝ擾���_�����t�@�C���ɏ�������

	GameObjects mGameObjects;
	std::unique_ptr<class Timer> mTimer;
	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class SoundEvent> mBGMSound;
	std::unique_ptr<class SoundEvent> mGameOverSound;
	std::unique_ptr<class ModelManager> mModelManager;
	std::unique_ptr<class JsonFile> mJsonFile;
	std::unique_ptr<class StageCreater> mStageCreater;
	std::unique_ptr<struct KeyCounter> mKeyCounter;
	std::string mJsonStageObjectName;
	Medal mMedal;
	bool mIsClear;
	bool mWasCreatedResult;
	int mElapsedTimeAfterClear;
};

