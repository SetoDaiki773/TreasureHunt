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
	EEnter,  // 相手トリガーが侵入した
	EOut,     // 相手トリガーとの接触が終わった
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
/// [機能] 動かない剛体判定を付与出来る。
//================================================
class StaticRigidBody : public Component{
public:
	StaticRigidBody(class GameObject& owner);
	~StaticRigidBody();

	static void ReflectAllPosToPhysXWorld();
	static void ReflectAllPosOfPhysXWorld();

	//---------------------------------------------------------------------
	/// [機能] 半径、物理マテリアル、ローカル座標をもとに当たり判定を作る
	//---------------------------------------------------------------------
	void AttachSphere(float radius, PhysicsMaterial material = {},const char* name = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBox(float halfX, float halfY, float halfZ, PhysicsMaterial material = {},const char* name = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachSphereTrigger(float radius, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBoxTrigger(float halfX, float halfY, float halfZ, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	//-------------------------------------------------------------------------------------------------
	/// [機能] triggerNameと一致するtriggerがなにかと衝突した時に呼び出す関数を第一引数に指定する
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
/// [機能] 重力などの影響を受ける、動く剛体判定を付与する
//========================================================
class DynamicRigidBody :public Component{
public:
	DynamicRigidBody(class GameObject& owner,float mass = 1.0f);
	~DynamicRigidBody();
	static void ReflectAllPosToPhysXWorld();
	static void ReflectAllPosOfPhysXWorld();

	void Update()override;
	
	//-----------------------------------
	/// [機能] 速度を完全にリセットする
	//-----------------------------------
	void ResetVelocity();
	//---------------------------------
	/// [機能] X軸速度のみリセットする
	//---------------------------------
	void ResetVelocityX();
	//---------------------------------
	// [機能] Y軸速度のみリセットする
	//---------------------------------
	void ResetVelocityY();
	//---------------------------------
	/// [機能] Z軸速度のみリセットする
	//---------------------------------
	void ResetVelocityZ();

	//---------------------------
	// [機能] 着地中か判定
	//---------------------------
	bool IsLanding();
	//----------------------------------------------------------------------------
	/// [機能] 他オブジェクトとのめり込みを防ぐためのBoxを取り付ける。
	///        本体の当たり判定より少し大きめにしておく必要がある。
	//----------------------------------------------------------------------------
	void AttachPreventingPenetrationBox(float halfX, float halfY, float halfZ, const DirectX::XMFLOAT3& localPos = {});
	//---------------------------------------------------------------------
	/// [機能] 半径、物理マテリアル、ローカル座標をもとに当たり判定を作る
	//---------------------------------------------------------------------
	void AttachSphere(float radius, PhysicsMaterial material = {},const char* name = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBox(float halfX, float halfY, float halfZ, PhysicsMaterial material = {},const char* name = "Object",DirectX::XMFLOAT3 localPos = {});
	void AttachSphereTrigger(float radius, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	void AttachBoxTrigger(float halfX, float halfY, float halfZ, PhysicsMaterial material = {}, const char* triggerName = "Object", DirectX::XMFLOAT3 localPos = {});
	//-------------------------------------------------------------------------------------------------
	/// [機能] triggerNameと一致するtriggerがなにかと衝突した時に呼び出す関数を第一引数に指定する
	//--------------------------------------------------------------------------------------------------
	void SetTriggerFunc(std::function<void(const HitObjectInfo&)>, const std::string& triggerName);
	//----------------------------------------------
	/// [機能] 速度を加える
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
