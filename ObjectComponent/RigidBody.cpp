#include "RigidBody.h"
#include "GameObject/GameObject.h"
#include "Common/VectorOperation.h"
#include "Common/MyMath.h"

std::vector<StaticRigidBody*> StaticRigidBody::mAllRigidStatic;
std::vector<DynamicRigidBody*> DynamicRigidBody::mAllRigidDynamic;


namespace {
	struct PhysX {
	public:
		PhysX();
		~PhysX();
		// PhysX内で利用するアロケーター
		physx::PxDefaultAllocator mDefaultAllocator;
		// エラー時用のコールバックでエラー内容が入ってる
		physx::PxDefaultErrorCallback mDefaultErrorCallback;
		// 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
		physx::PxFoundation* mFoundation = nullptr;
		// 実際に物理演算を行う
		physx::PxPhysics* mPhysics = nullptr;
		// シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
		physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;
		// シミュレーションする空間の単位でActorの追加などもここで行う
		physx::PxScene* mPhysXScene = nullptr;
		// PVDと通信する際に必要
		physx::PxPvd* mPvd = nullptr;

	};

	auto gDefaultTriggerFunc = [](const HitObjectInfo&) {};
	PhysX gPhysX = {};
	// トリガーが剛体と当たったときに呼び出す関数たち
	std::unordered_map<physx::PxShape*, std::function<void(const HitObjectInfo&)>> gTriggerFuncs;

	// PhsxのベクトルをXMFLOAT3に変換
	DirectX::XMFLOAT3 ToXMFLOAT3(const physx::PxVec3& pxVec) {
		return DirectX::XMFLOAT3(pxVec.x, pxVec.y, pxVec.z);
	}
	// XMFLOAT3をPhysXのベクトルに変換
	physx::PxVec3 ToPxVec(const DirectX::XMFLOAT3& xmfloat3) {
		return physx::PxVec3(xmfloat3.x, xmfloat3.y, xmfloat3.z);
	}
	// PhysXのペアフラグをトリガーイベント型に変換
	TriggerEvent ToMyTriggerEvent(physx::PxPairFlag::Enum flag) {
		std::unordered_map<physx::PxPairFlag::Enum, TriggerEvent> toMyTriggerEventMap;
		toMyTriggerEventMap[physx::PxPairFlag::eNOTIFY_TOUCH_FOUND] = TriggerEvent::EEnter; // ちょうど判定に入った
		toMyTriggerEventMap[physx::PxPairFlag::eNOTIFY_TOUCH_LOST] = TriggerEvent::EOut;    // ちょうど判定から離れた
		auto iter = toMyTriggerEventMap.find(flag);
		if (iter != toMyTriggerEventMap.end()) return iter->second;  // PxPairFlagをTriggerEventに変更したものを返す。
		return TriggerEvent::EDefault;                               // PxPairFlagをTriggerEventに変更できなっかったらデフォルトを返す
	}

	
	//-------------------------------------------------------------------------------------
	/// PxSimulationEventCallbackは剛体と当たったときなどにコールバックされる
	/// コールバックされる関数は仮想関数なのでこちら側で実装する
	/// このプログラムではとりあえず、トリガーと剛体が当たった時にコールバックされる
	/// onTrigger()関数のみをoverrideする
	//-------------------------------------------------------------------------------------
	struct Trigger : public physx::PxSimulationEventCallback {
	public:
		//-------------------------------------------------------------------------
		// [機能] 剛体がトリガーに侵入した時と、離れた時にコールバックされる関数
		//-------------------------------------------------------------------------
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 hitCount)override {
			// 当たったトリガーの数だけ繰り返す
			for (physx::PxU32 i = 0; i < hitCount; i++) {
				DirectX::XMFLOAT3 pos = ToXMFLOAT3(pairs[i].otherActor->is <physx::PxRigidActor>()->getGlobalPose().p);
				// 当たった剛体の情報を設定する。
				HitObjectInfo info{ pairs[i].otherShape->getName(), ToMyTriggerEvent(pairs[i].status),pos};
				// 当たったトリガー形状のアドレスをキーとして、そのトリガーが持つ関数を呼び出す。
				gTriggerFuncs[pairs[i].triggerShape](info);
			}
		}

		// 他の関数は実装してない
		void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32)override {}
		void onWake(physx::PxActor**, physx::PxU32)override {}
		void onSleep(physx::PxActor**, physx::PxU32)override {}
		void onContact(const physx::PxContactPairHeader&, const physx::PxContactPair*, physx::PxU32)override {}
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)override {}
	};

	//-----------------------------------------------------
	//
	// PxRigidActorに剛体とか、トリガーとかを紐づける関数群

	void AttachSphere(physx::PxRigidActor* actor, float radius, PhysicsMaterial material,const char* name,DirectX::XMFLOAT3 localPos) {
		physx::PxShape* sphere = gPhysX.mPhysics->createShape(
			physx::PxSphereGeometry(radius),
			*gPhysX.mPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.bounce)
		);
		sphere->setName(name);
		SetLocalPose(sphere,localPos);
		actor->attachShape(*sphere);
	}
	void AttachBox(physx::PxRigidActor* actor, float halfX, float halfY, float halfZ,PhysicsMaterial material,const char* name,DirectX::XMFLOAT3 localPos) {
		physx::PxShape* box = gPhysX.mPhysics->createShape(
			physx::PxBoxGeometry(halfX,halfY,halfZ),
			*gPhysX.mPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.bounce)
		);
		box->setName(name);
		SetLocalPose(box,localPos);
		actor->attachShape(*box);
	}
	void AttachSphereTrigger(physx::PxRigidActor* actor,float radius, PhysicsMaterial material, const char* triggerName, DirectX::XMFLOAT3 localPos) {
		physx::PxShape* sphere = gPhysX.mPhysics->createShape(
			physx::PxSphereGeometry(radius),
			*gPhysX.mPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.bounce)
		);
		sphere->setName(triggerName);
		SetLocalPose(sphere, localPos);
		sphere->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false); // トリガーはシミュレーションをfalseにする必要がある
		sphere->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);     // トリガーなのでトリガーフラグをオンに
		gTriggerFuncs[sphere] = gDefaultTriggerFunc;                   // 衝突時に呼び出す関数にはデフォルトで、何もしない関数を紐づけておく
		actor->attachShape(*sphere);
	}
	void AttachBoxTrigger(physx::PxRigidActor* actor,float halfX, float halfY, float halfZ, PhysicsMaterial material, const char* triggerName,DirectX::XMFLOAT3 localPos) {
		physx::PxShape* box = gPhysX.mPhysics->createShape(
			physx::PxBoxGeometry(halfX, halfY, halfZ),
			*gPhysX.mPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.bounce)
		);
		box->setName(triggerName);
		SetLocalPose(box, localPos);
		box->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
		box->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
		gTriggerFuncs[box] = gDefaultTriggerFunc;
		actor->attachShape(*box);
	}
	//
	//---------------------------------------------------------------------

	void SetLocalPose(physx::PxShape* shape,const DirectX::XMFLOAT3& localPos) {
		physx::PxTransform trans(physx::PxIdentity);
		trans.p = ToPxVec(localPos);
		shape->setLocalPose(trans);
	}
}

void PhysXWorld::Update() {
	// 1フレーム分シミュレーションを進める。1/60だと、いまいちだから、2倍速で物理を進める
	gPhysX.mPhysXScene->simulate(1.f / 30.f);
	// PhysXの処理が終わるまで待つ
	gPhysX.mPhysXScene->fetchResults(true);
}

//------------------------------------------------------------------
//  物理世界とゲーム世界の位置情報を一致させる関数群
//------------------------------------------------------------------
void DynamicRigidBody::ReflectAllPosToPhysXWorld() {
	// 全てのDynamicRigidBodyの位置を物理世界に反映する
	for (auto rigid : mAllRigidDynamic) {
		rigid->ReflectPosToPhysXWorld();
	}
}
void DynamicRigidBody::ReflectAllPosOfPhysXWorld() {
	// 全てのDynamicRigidBodyの物理世界の位置をゲーム世界に反映する
	for (auto rigid : mAllRigidDynamic) {
		rigid->ReflectPosOfPhysXWorld();
	}
}
void StaticRigidBody::ReflectAllPosToPhysXWorld() {
	// Dynamicと同じように位置を反映する
	for (auto rigid : mAllRigidStatic) {
		rigid->ReflectPosToPhysXWorld();
	}
}
void StaticRigidBody::ReflectAllPosOfPhysXWorld() {
	// Dynamicと同じように位置を反映する
	for (auto rigid : mAllRigidStatic) {
		rigid->ReflectPosOfPhysXWorld();
	}
}

//----------------------------
//  コンストラクタ
//----------------------------
DynamicRigidBody::DynamicRigidBody(GameObject& owner, float mass)
	:mOwner(owner)
	,mVelocity()
	,mNextAdditionalPos(0.0f, 0.0f, 0.0f)
	,mPosBeforeUpdatePhysX(owner.GetPos())
{
	// 物理世界の位置を設定して、物理世界に剛体を追加
	physx::PxTransform trans(physx::PxIdentity);
	trans.p = ToPxVec(owner.GetPos());
	mRigidDynamic = gPhysX.mPhysics->createRigidDynamic(trans);
	mRigidDynamic->setMass(mass);
	gPhysX.mPhysXScene->addActor(*mRigidDynamic);
	mAllRigidDynamic.push_back(this);
}


StaticRigidBody::StaticRigidBody(GameObject& owner)
	:mOwner(owner)
{
	// 物理世界の位置を設定したのち、物理世界に作ったBodyを追加
	physx::PxTransform trans(physx::PxIdentity);
	trans.p = ToPxVec(owner.GetPos());
	mRigidStatic = gPhysX.mPhysics->createRigidStatic(trans);
	gPhysX.mPhysXScene->addActor(*mRigidStatic);
	mAllRigidStatic.push_back(this);
}


DynamicRigidBody::~DynamicRigidBody() {
	gPhysX.mPhysXScene->removeActor(*mRigidDynamic);  // 自分を物理世界から消す
	std::erase_if(mAllRigidDynamic, [this](DynamicRigidBody* rigid) { return rigid == this; }); // AllRigidDynamicから自分を消す
}

StaticRigidBody::~StaticRigidBody() {
	gPhysX.mPhysXScene->removeActor(*mRigidStatic);  // 自分を物理世界から消す
	std::erase_if(mAllRigidStatic, [this](StaticRigidBody* rigid) {return rigid = this; });  // AllRigidStaticから自分を消す
}

void DynamicRigidBody::Update() {
	mOwner.AddPos(mNextAdditionalPos); // 貫通対策用の加算位置を加算する
	mNextAdditionalPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 0に戻す
}



//---------------------------------------
// mRigidStaticに形を紐づける関数群
//---------------------------------------
void StaticRigidBody::AttachSphere(float radius, PhysicsMaterial material,const char* name,DirectX::XMFLOAT3 localPos) {
	::AttachSphere(mRigidStatic,radius,material,name,localPos);
}
void StaticRigidBody::AttachBox(float halfX, float halfY, float halfZ, PhysicsMaterial material,const char* name,DirectX::XMFLOAT3 localPos) {
	::AttachBox(mRigidStatic, halfX, halfY, halfZ, material, name, localPos);
}

void StaticRigidBody::AttachSphereTrigger(float radius, PhysicsMaterial material, const char* triggerName, DirectX::XMFLOAT3 localPos) {
	::AttachSphereTrigger(mRigidStatic, radius, material, triggerName, localPos);

}

void StaticRigidBody::AttachBoxTrigger(float halfX, float halfY, float halfZ, PhysicsMaterial material, const char* triggerName, DirectX::XMFLOAT3 localPos) {
	::AttachBoxTrigger(mRigidStatic,halfX,halfY,halfZ,material,triggerName,localPos);
}



//----------------------------------------------------------------
// triggerNameと名前が一致したトリガーにトリガー関数を設定する
//----------------------------------------------------------------
void StaticRigidBody::SetTriggerFunc(std::function<void(const HitObjectInfo&)> triggerFunc, const std::string& triggerName) {
	physx::PxU32 shapeCount = mRigidStatic->getNbShapes();    // mRigidBodyに割り当てられたシェイプの数を取得
	physx::PxShape** shapes = new physx::PxShape * [shapeCount];
	mRigidStatic->getShapes(shapes, shapeCount); // shapesにmRigidBodyに割り当てられたシェイプを取得
	// triggerNameとシェイプの名前が一致したら、一致したシェイプのアドレスと関数をhashで紐づける
	for (physx::PxU32 i = 0; i < shapeCount; ++i) {
		if (shapes[i]->getName() == triggerName) {
			gTriggerFuncs[shapes[i]] = triggerFunc;
		}
	}
	delete[] shapes;
}

//--------------------------------------
// 物理世界に位置を反映する
//--------------------------------------
void StaticRigidBody::ReflectPosToPhysXWorld() {
	physx::PxTransform nowTrans(physx::PxIdentity);
	nowTrans.p = physx::PxVec3(ToPxVec(mOwner.GetPos()));
	mRigidStatic->setGlobalPose(nowTrans);
}

//-----------------------------------
//  物理世界の位置を反映する
//-----------------------------------
void StaticRigidBody::ReflectPosOfPhysXWorld() {
	DirectX::XMFLOAT3 pos = ToXMFLOAT3(mRigidStatic->getGlobalPose().p);
	mOwner.AddPos(pos - mOwner.GetPos());
}

//----------------------------------------------------------------
// 他の剛体とのめり込みを無くすためのトリガーを取り付ける関数
//----------------------------------------------------------------
void DynamicRigidBody::AttachPreventingPenetrationBox(float hx,float hy,float hz,const DirectX::XMFLOAT3& localPos) {
	::AttachBoxTrigger(mRigidDynamic, hx, hy, hz, {}, "PreventingBox", localPos);
	// めり込みを解消する関数
	auto preventingFunc = [this](const HitObjectInfo& info) {
		// Objectと当たったら次のフレームで、前の移動した分の2.5倍後ろに戻すことでめり込みを解消する
		if (info.triggerEvent == TriggerEvent::EEnter && info.name == "Object") {
			mNextAdditionalPos = 2.5 * (mOwner.GetPrevFramePos() - mOwner.GetPos());
		}
	};
	SetTriggerFunc(preventingFunc, "PreventingBox"); // めり込み解消関数をトリガーに設定
}

//------------------------------------------------
// Dynamic剛体に計上を割り当てる
//------------------------------------------------
void DynamicRigidBody::AttachSphere(float radius, PhysicsMaterial material,const char* name,DirectX::XMFLOAT3 localPos) {
	::AttachSphere(mRigidDynamic, radius, material,name,localPos);
}
void DynamicRigidBody::AttachBox(float halfX, float halfY, float halfZ, PhysicsMaterial material,const char* name,DirectX::XMFLOAT3 localPos) {
	::AttachBox(mRigidDynamic,halfX,halfY,halfZ,material,name,localPos);
}

void DynamicRigidBody::AttachSphereTrigger(float radius, PhysicsMaterial material, const char* triggerName, DirectX::XMFLOAT3 localPos) {
	::AttachSphereTrigger(mRigidDynamic, radius, material, triggerName, localPos);
}

void DynamicRigidBody::AttachBoxTrigger(float halfX, float halfY, float halfZ, PhysicsMaterial material, const char* triggerName, DirectX::XMFLOAT3 localPos) {
	::AttachBoxTrigger(mRigidDynamic,halfX,halfY,halfX,material,triggerName,localPos);
}

//-------------------------------------------------------------------------------
//  triggerNameと一致したトリガーが何かに当たった時に呼び出す関数を設定する
//-------------------------------------------------------------------------------
void DynamicRigidBody::SetTriggerFunc(std::function<void(const HitObjectInfo&)> triggerFunc, const std::string& triggerName) {
	physx::PxU32 shapeCount = mRigidDynamic->getNbShapes();    // mRigidBodyに割り当てられたシェイプの数を取得
	physx::PxShape** shapes = new physx::PxShape * [shapeCount];
	mRigidDynamic->getShapes(shapes, shapeCount); // shapesにmRigidBodyに割り当てられたシェイプを取得
	// triggerNameとシェイプの名前が一致したら、一致したシェイプのアドレスと関数をhashで紐づける
	for (physx::PxU32 i = 0; i < shapeCount; ++i) {
		if (shapes[i]->getName() == triggerName) {
			gTriggerFuncs[shapes[i]] = triggerFunc;
		}
	}
	delete[] shapes;
}

//-----------------------------------------
//  速度操作系関数
//-----------------------------------------
void DynamicRigidBody::AddForce(const DirectX::XMFLOAT3& velocity) {
	mRigidDynamic->addForce(ToPxVec(velocity),physx::PxForceMode::eVELOCITY_CHANGE);
}

void DynamicRigidBody::ResetVelocity() {
	mRigidDynamic->setLinearVelocity(physx::PxVec3(0.0f,0.0f,0.0f));
}

void DynamicRigidBody::ResetVelocityZ() {
	physx::PxVec3 velocity = mRigidDynamic->getLinearVelocity();
	velocity.z = 0.0f;
	mRigidDynamic->setLinearVelocity(velocity);
}

void DynamicRigidBody::ResetVelocityX() {
	physx::PxVec3 velocity = mRigidDynamic->getLinearVelocity();
	velocity.x = 0.0f;
	mRigidDynamic->setLinearVelocity(velocity);
}

void DynamicRigidBody::ResetVelocityY() {
	physx::PxVec3 velocity = mRigidDynamic->getLinearVelocity();
	velocity.y = 0.0f;
	mRigidDynamic->setLinearVelocity(velocity);
}

bool DynamicRigidBody::IsLanding() {
	// 下方向への力がほぼ0なら着地しているとする
	if (Math::NearEqualF(mRigidDynamic->getLinearVelocity().y * 0.1f, 0.0f)) return true;
	return false;
}

//-----------------------------------------
// 物理世界に位置を反映する
//-----------------------------------------
void DynamicRigidBody::ReflectPosToPhysXWorld() {
	mPosBeforeUpdatePhysX = mOwner.GetPos();
	physx::PxTransform nowTrans(physx::PxIdentity);
	nowTrans.p = physx::PxVec3(ToPxVec(mOwner.GetPos()));
	mRigidDynamic->setGlobalPose(nowTrans);
}
//-----------------------------------------
// 物理世界の位置をゲームに反映する
//-----------------------------------------
void DynamicRigidBody::ReflectPosOfPhysXWorld() {
	// physxをupdateした後にに変化したownerの位置。これを最後に加算することで、トリガー関数による位置変化を反映する。
	DirectX::XMFLOAT3 deltaPosByTriggerFunc = mOwner.GetPos() - mPosBeforeUpdatePhysX;

	DirectX::XMFLOAT3 pos = ToXMFLOAT3(mRigidDynamic->getGlobalPose().p); // 物理世界の位置から
	DirectX::XMFLOAT3 additionalPos = pos - mOwner.GetPos();              // ゲーム世界の位置を引いたものが、ゲーム世界に加算する位置変化
	// xz方向への位置変化が小さいなら0にすることで、ブロック先端に乗ったときに滑り落ちるのをある程度防ぐ。
	if (Math::NearEqualF(additionalPos.x * 2.0f, 0.0f)) additionalPos.x = 0.0f;
	if (Math::NearEqualF(additionalPos.z * 2.0f, 0.0f)) additionalPos.z = 0.0f;
	mOwner.AddPos(additionalPos);         // 加算して物理世界の位置をゲームに反映する。

	mOwner.AddPos(deltaPosByTriggerFunc); // トリガー関数による位置変化を加算する。
}



//-----------------------------
// PhysX構造体の関数群
//-----------------------------
PhysX::PhysX() {

	// Foundationのインスタンス化
	if (mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator, mDefaultErrorCallback); !mFoundation) {
		assert(!"PhysXFoundation初期化失敗");
		return;
	}
	// PVDと接続する設定
	if (mPvd = physx::PxCreatePvd(*mFoundation); mPvd) {
		// PVD側のデフォルトポートは5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	}
	// Physicsのインスタンス化
	if (mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true, mPvd); !mPhysics) {
		assert(!"PhyX初期化失敗");
		return;
	}
	// 拡張機能用
	if (!PxInitExtensions(*mPhysics, mPvd)) {
		assert(!"physX拡張機能設定失敗");
		return;
	}
	// 処理に使うスレッドを指定する
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
	// 空間の設定
	physx::PxSceneDesc scene_desc(mPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0.0f, -20.0f, 0.0f);
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	scene_desc.cpuDispatcher = mDispatcher;
	scene_desc.simulationEventCallback = new Trigger{};
	// 空間のインスタンス化
	mPhysXScene = mPhysics->createScene(scene_desc);
	// PVDの表示設定
	physx::PxPvdSceneClient* pvd_client;
	if (pvd_client = mPhysXScene->getScenePvdClient(); pvd_client) {
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	
}

PhysX::~PhysX() {
	PxCloseExtensions();
	mPhysXScene->release();
	mDispatcher->release();
	mPhysics->release();
	if (mPvd) {
		mPvd->disconnect();
		physx::PxPvdTransport* transport = mPvd->getTransport();
		mPvd->release();
		transport->release();
	}
	mFoundation->release();
}