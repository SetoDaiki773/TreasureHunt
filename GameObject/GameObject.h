#pragma once
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <DirectXMath.h>
#include <typeIndex>


// �O���錾
namespace Object {
	enum Type;
};


//=====================================================
/// [�@�\] �I�u�W�F�N�g�̃N���X�ł��B
/// 3D�I�u�W�F�N�g������Ƃ��͂��̃N���X���g�p���܂�
//=====================================================
class GameObject
{
public:
	enum class State {
		EActive,
		EDead,
		EStop,
		EUnDraw,
		EStopUndraw,
		EOther
	};

	
	//---------------------------------------------------------------------------------------------------
	/// [�@�\] �R���X�g���N�^�B������p����object�̏����������܂��B�ʒu�A��]�A�g�嗦������ł��܂�
	/// [�����P] �I�u�W�F�N�g�̌�����(���_�⃋�[�g�V�O�l�`���Ȃ�)�̃\�[�X�ł�
	/// [�����Q] ���[���h���W
	//----------------------------------------------------------------------------------------------------
	GameObject(
		std::shared_ptr<class Model> model,
		class WorldTransform&& world,
		int expectedNumComponents = 5
	);

	void Update();

	//---------------------------------------------------
	///[�@�\] �I�u�W�F�N�g��`�悵�܂�
	///[����] �J����
	//---------------------------------------------------
	void Render(const class Camera& camera); 


	//---------------------------------------------------
	/// [�@�\] �e���e�N�X�`���Ƀ}�b�v����
	/// [����] �J����
	//---------------------------------------------------
	void ShadowMap(const class Camera& camera);

	//---------------------------------------------------
	/// [�@�\] �C�x���g�𑗐M����
	/// [����] �C�x���g
	//---------------------------------------------------
	void ReceiveEvent(const class Event& event);


	//---------------------------------------------------
	///[�@�\] �I�u�W�F�N�g�������Ă��Ă悢�̂��擾
	///[�߂�l] �I�u�W�F�N�g�������Ă悢�̂�
	//---------------------------------------------------
	bool NeedErase()const;

	
	//--------------------------------------------------
	/// ���[���h�g�����X�t�H�[���̃Q�b�^�[�Z�b�^�[
	//--------------------------------------------------
	void SetWorld(const class WorldTransform& world);
	const class WorldTransform& GetWorld()const;
	void SetPos(const DirectX::XMFLOAT3& pos);
	const DirectX::XMFLOAT3& GetPos()const;
	void AddPos(const DirectX::XMFLOAT3& pos);
	void SetQuaternion(const DirectX::XMVECTOR& quaternion);
	DirectX::XMVECTOR GetQuaternion()const;
	void SetScale(const DirectX::XMFLOAT3& scale);
	DirectX::XMFLOAT3 GetScale()const;
	void SetState(State state);
	State GetState();

	DirectX::XMFLOAT3 GetPrevFramePos();

	template <class CompType>
	void AddSharedComponent(std::shared_ptr<CompType> component) {
		mComponents.emplace(typeid(CompType), component); // �R���|�[�l���g�ǉ�
	}

	template <class CompType>
	void AddRValueComponent(CompType&& component) {
		mComponents.emplace(typeid(CompType), std::make_shared<CompType>(std::move(component))); // �R���|�[�l���g�ǉ�
	}
	template<class CompType>
	void RemoveComponent() {
		mComponents.erase(typeid(CompType)); // ����������A�����^�̃R���|�[�l���g������
	}
private:
	
	std::unordered_map<std::type_index,std::shared_ptr<class Component>> mComponents;
	State mState;                                                  // �I�u�W�F�N�g�̏��
	std::shared_ptr<class Model> mResource;                        // ������
	std::unique_ptr<class WorldTransform> mWorld;                  // ���[���h�g�����X�t�H�[��
	DirectX::XMFLOAT3 mPrevFramePos;
};

using GameObjectPtrS = std::shared_ptr<GameObject>;


