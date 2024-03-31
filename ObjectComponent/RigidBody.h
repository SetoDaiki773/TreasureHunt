#pragma once
#include "Component.h"
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include "PxPhysicsAPI.h"
#include <iostream>
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "SceneQuery_static_64.lib")
#pragma comment(lib, "SimulationController_static_64.lib")


enum class TriggerEvent {
	EEnter,  // ����g���K�[���N������
	EOut,     // ����g���K�[�Ƃ̐ڐG���I�����
	EPresist,
	EDefault
};

struct HitObjectInfo {
	std::string name;
	TriggerEvent triggerEvent;
	DirectX::XMFLOAT3 pos;
};


struct PhysicsMaterial {
	float staticFriction = 1.0f;
	float dynamicFriction = 1.0f;
	float bounce = 0.0f;
};

namespace PhysXWorld{
	void Update();
}

//================================================
/// [�@�\] �����Ȃ����̔����t�^�o����B
//================================================
class StaticRigidBody : public Component{
public:
	StaticRigidBody(class GameObject& owner);
	~StaticRigidBody();

	static void ReflectAllPosToPhysXWorld();
	static void ReflectAllPosOfPhysXWorld();

	//---------------------------------------------------------------------
	/// [�@�\] ���a�A�����}�e���A���A���[�J�����W�����Ƃɓ����蔻������
	//---------------------------------------------------------------------
	void AttachSphere(float radius, PhysicsMaterial material = {},const char* name = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBox(float halfX, float halfY, float halfZ, PhysicsMaterial material = {},const char* name = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachSphereTrigger(float radius, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBoxTrigger(float halfX, float halfY, float halfZ, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	//-------------------------------------------------------------------------------------------------
	/// [�@�\] triggerName�ƈ�v����trigger���Ȃɂ��ƏՓ˂������ɌĂяo���֐���������Ɏw�肷��
	//--------------------------------------------------------------------------------------------------
	void SetTriggerFunc(std::function<void(const HitObjectInfo&)> triggerFunc, const std::string& triggerName);

	void ReflectPosToPhysXWorld();
	void ReflectPosOfPhysXWorld();
private:
	physx::PxRigidStatic* mRigidStatic;
	class GameObject& mOwner;
	static std::vector<StaticRigidBody*> mAllRigidStatic;
};

//========================================================
/// [�@�\] �d�͂Ȃǂ̉e�����󂯂�A�������̔����t�^����
//========================================================
class DynamicRigidBody :public Component{
public:
	DynamicRigidBody(class GameObject& owner,float mass = 1.0f);
	~DynamicRigidBody();
	static void ReflectAllPosToPhysXWorld();
	static void ReflectAllPosOfPhysXWorld();

	void Update()override;
	
	//-----------------------------------
	/// [�@�\] ���x�����S�Ƀ��Z�b�g����
	//-----------------------------------
	void ResetVelocity();
	//---------------------------------
	/// [�@�\] X�����x�̂݃��Z�b�g����
	//---------------------------------
	void ResetVelocityX();
	//---------------------------------
	// [�@�\] Y�����x�̂݃��Z�b�g����
	//---------------------------------
	void ResetVelocityY();
	//---------------------------------
	/// [�@�\] Z�����x�̂݃��Z�b�g����
	//---------------------------------
	void ResetVelocityZ();

	//---------------------------
	// [�@�\] ���n��������
	//---------------------------
	bool IsLanding();
	//----------------------------------------------------------------------------
	/// [�@�\] ���I�u�W�F�N�g�Ƃ̂߂荞�݂�h�����߂�Box�����t����B
	///        �{�̂̓����蔻���菭���傫�߂ɂ��Ă����K�v������B
	//----------------------------------------------------------------------------
	void AttachPreventingPenetrationBox(float halfX, float halfY, float halfZ, const DirectX::XMFLOAT3& localPos = {});
	//---------------------------------------------------------------------
	/// [�@�\] ���a�A�����}�e���A���A���[�J�����W�����Ƃɓ����蔻������
	//---------------------------------------------------------------------
	void AttachSphere(float radius, PhysicsMaterial material = {},const char* name = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBox(float halfX, float halfY, float halfZ, PhysicsMaterial material = {},const char* name = "Object",DirectX::XMFLOAT3 localPos = {});
	void AttachSphereTrigger(float radius, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBoxTrigger(float halfX, float halfY, float halfZ, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	//-------------------------------------------------------------------------------------------------
	/// [�@�\] triggerName�ƈ�v����trigger���Ȃɂ��ƏՓ˂������ɌĂяo���֐���������Ɏw�肷��
	//--------------------------------------------------------------------------------------------------
	void SetTriggerFunc(std::function<void(const HitObjectInfo&)>, const std::string& triggerName);
	//----------------------------------------------
	/// [�@�\] ���x��������
	//----------------------------------------------
	void AddForce(const DirectX::XMFLOAT3& velocity);

	void ReflectPosToPhysXWorld();
	void ReflectPosOfPhysXWorld();
private:
	physx::PxRigidDynamic* mRigidDynamic;
	class GameObject& mOwner;
	bool mIsValid;
	DirectX::XMFLOAT3 mPosBeforeUpdatePhysX;
	static std::vector<DynamicRigidBody*> mAllRigidDynamic;
	DirectX::XMFLOAT3 mVelocity;
	DirectX::XMFLOAT3 mNextAdditionalPos;
};

using StaticRigidPtr = std::shared_ptr<StaticRigidBody>;
using DynamicRigidPtr = std::shared_ptr<DynamicRigidBody>;
