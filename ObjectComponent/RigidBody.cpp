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
		// PhysX���ŗ��p����A���P�[�^�[
		physx::PxDefaultAllocator mDefaultAllocator;
		// �G���[���p�̃R�[���o�b�N�ŃG���[���e�������Ă�
		physx::PxDefaultErrorCallback mDefaultErrorCallback;
		// ��ʃ��x����SDK(PxPhysics�Ȃ�)���C���X�^���X������ۂɕK�v
		physx::PxFoundation* mFoundation = nullptr;
		// ���ۂɕ������Z���s��
		physx::PxPhysics* mPhysics = nullptr;
		// �V�~�����[�V�������ǂ��������邩�̐ݒ�Ń}���`�X���b�h�̐ݒ���ł���
		physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;
		// �V�~�����[�V���������Ԃ̒P�ʂ�Actor�̒ǉ��Ȃǂ������ōs��
		physx::PxScene* mPhysXScene = nullptr;
		// PVD�ƒʐM����ۂɕK�v
		physx::PxPvd* mPvd = nullptr;

	};

	auto gDefaultTriggerFunc = [](const HitObjectInfo&) {};
	PhysX gPhysX = {};
	// �g���K�[�����̂Ɠ��������Ƃ��ɌĂяo���֐�����
	std::unordered_map<physx::PxShape*, std::function<void(const HitObjectInfo&)>> gTriggerFuncs;

	// Phsx�̃x�N�g����XMFLOAT3�ɕϊ�
	DirectX::XMFLOAT3 ToXMFLOAT3(const physx::PxVec3& pxVec) {
		return DirectX::XMFLOAT3(pxVec.x, pxVec.y, pxVec.z);
	}
	// XMFLOAT3��PhysX�̃x�N�g���ɕϊ�
	physx::PxVec3 ToPxVec(const DirectX::XMFLOAT3& xmfloat3) {
		return physx::PxVec3(xmfloat3.x, xmfloat3.y, xmfloat3.z);
	}
	// PhysX�̃y�A�t���O���g���K�[�C�x���g�^�ɕϊ�
	TriggerEvent ToMyTriggerEvent(physx::PxPairFlag::Enum flag) {
		std::unordered_map<physx::PxPairFlag::Enum, TriggerEvent> toMyTriggerEventMap;
		toMyTriggerEventMap[physx::PxPairFlag::eNOTIFY_TOUCH_FOUND] = TriggerEvent::EEnter; // ���傤�ǔ���ɓ�����
		toMyTriggerEventMap[physx::PxPairFlag::eNOTIFY_TOUCH_LOST] = TriggerEvent::EOut;    // ���傤�ǔ��肩�痣�ꂽ
		auto iter = toMyTriggerEventMap.find(flag);
		if (iter != toMyTriggerEventMap.end()) return iter->second;  // PxPairFlag��TriggerEvent�ɕύX�������̂�Ԃ��B
		return TriggerEvent::EDefault;                               // PxPairFlag��TriggerEvent�ɕύX�ł��Ȃ���������f�t�H���g��Ԃ�
	}

	
	//-------------------------------------------------------------------------------------
	/// PxSimulationEventCallback�͍��̂Ɠ��������Ƃ��ȂǂɃR�[���o�b�N�����
	/// �R�[���o�b�N�����֐��͉��z�֐��Ȃ̂ł����瑤�Ŏ�������
	/// ���̃v���O�����ł͂Ƃ肠�����A�g���K�[�ƍ��̂������������ɃR�[���o�b�N�����
	/// onTrigger()�֐��݂̂�override����
	//-------------------------------------------------------------------------------------
	struct Trigger : public physx::PxSimulationEventCallback {
	public:
		//-------------------------------------------------------------------------
		// [�@�\] ���̂��g���K�[�ɐN���������ƁA���ꂽ���ɃR�[���o�b�N�����֐�
		//-------------------------------------------------------------------------
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 hitCount)override {
			// ���������g���K�[�̐������J��Ԃ�
			for (physx::PxU32 i = 0; i < hitCount; i++) {
				DirectX::XMFLOAT3 pos = ToXMFLOAT3(pairs[i].otherActor->is <physx::PxRigidActor>()->getGlobalPose().p);
				// �����������̂̏���ݒ肷��B
				HitObjectInfo info{ pairs[i].otherShape->getName(), ToMyTriggerEvent(pairs[i].status),pos};
				// ���������g���K�[�`��̃A�h���X���L�[�Ƃ��āA���̃g���K�[�����֐����Ăяo���B
				gTriggerFuncs[pairs[i].triggerShape](info);
			}
		}

		// ���̊֐��͎������ĂȂ�
		void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32)override {}
		void onWake(physx::PxActor**, physx::PxU32)override {}
		void onSleep(physx::PxActor**, physx::PxU32)override {}
		void onContact(const physx::PxContactPairHeader&, const physx::PxContactPair*, physx::PxU32)override {}
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)override {}
	};

	//-----------------------------------------------------
	//
	// PxRigidActor�ɍ��̂Ƃ��A�g���K�[�Ƃ���R�Â���֐��Q

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
		sphere->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false); // �g���K�[�̓V�~�����[�V������false�ɂ���K�v������
		sphere->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);     // �g���K�[�Ȃ̂Ńg���K�[�t���O���I����
		gTriggerFuncs[sphere] = gDefaultTriggerFunc;                   // �Փˎ��ɌĂяo���֐��ɂ̓f�t�H���g�ŁA�������Ȃ��֐���R�Â��Ă���
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
	// 1�t���[�����V�~�����[�V������i�߂�B1/60���ƁA���܂���������A2�{���ŕ�����i�߂�
	gPhysX.mPhysXScene->simulate(1.f / 30.f);
	// PhysX�̏������I���܂ő҂�
	gPhysX.mPhysXScene->fetchResults(true);
}

//------------------------------------------------------------------
//  �������E�ƃQ�[�����E�̈ʒu������v������֐��Q
//------------------------------------------------------------------
void DynamicRigidBody::ReflectAllPosToPhysXWorld() {
	// �S�Ă�DynamicRigidBody�̈ʒu�𕨗����E�ɔ��f����
	for (auto rigid : mAllRigidDynamic) {
		rigid->ReflectPosToPhysXWorld();
	}
}
void DynamicRigidBody::ReflectAllPosOfPhysXWorld() {
	// �S�Ă�DynamicRigidBody�̕������E�̈ʒu���Q�[�����E�ɔ��f����
	for (auto rigid : mAllRigidDynamic) {
		rigid->ReflectPosOfPhysXWorld();
	}
}
void StaticRigidBody::ReflectAllPosToPhysXWorld() {
	// Dynamic�Ɠ����悤�Ɉʒu�𔽉f����
	for (auto rigid : mAllRigidStatic) {
		rigid->ReflectPosToPhysXWorld();
	}
}
void StaticRigidBody::ReflectAllPosOfPhysXWorld() {
	// Dynamic�Ɠ����悤�Ɉʒu�𔽉f����
	for (auto rigid : mAllRigidStatic) {
		rigid->ReflectPosOfPhysXWorld();
	}
}

//----------------------------
//  �R���X�g���N�^
//----------------------------
DynamicRigidBody::DynamicRigidBody(GameObject& owner, float mass)
	:mOwner(owner)
	,mVelocity()
	,mNextAdditionalPos(0.0f, 0.0f, 0.0f)
	,mPosBeforeUpdatePhysX(owner.GetPos())
{
	// �������E�̈ʒu��ݒ肵�āA�������E�ɍ��̂�ǉ�
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
	// �������E�̈ʒu��ݒ肵���̂��A�������E�ɍ����Body��ǉ�
	physx::PxTransform trans(physx::PxIdentity);
	trans.p = ToPxVec(owner.GetPos());
	mRigidStatic = gPhysX.mPhysics->createRigidStatic(trans);
	gPhysX.mPhysXScene->addActor(*mRigidStatic);
	mAllRigidStatic.push_back(this);
}


DynamicRigidBody::~DynamicRigidBody() {
	gPhysX.mPhysXScene->removeActor(*mRigidDynamic);  // �����𕨗����E�������
	std::erase_if(mAllRigidDynamic, [this](DynamicRigidBody* rigid) { return rigid == this; }); // AllRigidDynamic���玩��������
}

StaticRigidBody::~StaticRigidBody() {
	gPhysX.mPhysXScene->removeActor(*mRigidStatic);  // �����𕨗����E�������
	std::erase_if(mAllRigidStatic, [this](StaticRigidBody* rigid) {return rigid = this; });  // AllRigidStatic���玩��������
}

void DynamicRigidBody::Update() {
	mOwner.AddPos(mNextAdditionalPos); // �ђʑ΍��p�̉��Z�ʒu�����Z����
	mNextAdditionalPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 0�ɖ߂�
}



//---------------------------------------
// mRigidStatic�Ɍ`��R�Â���֐��Q
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
// triggerName�Ɩ��O����v�����g���K�[�Ƀg���K�[�֐���ݒ肷��
//----------------------------------------------------------------
void StaticRigidBody::SetTriggerFunc(std::function<void(const HitObjectInfo&)> triggerFunc, const std::string& triggerName) {
	physx::PxU32 shapeCount = mRigidStatic->getNbShapes();    // mRigidBody�Ɋ��蓖�Ă�ꂽ�V�F�C�v�̐����擾
	physx::PxShape** shapes = new physx::PxShape * [shapeCount];
	mRigidStatic->getShapes(shapes, shapeCount); // shapes��mRigidBody�Ɋ��蓖�Ă�ꂽ�V�F�C�v���擾
	// triggerName�ƃV�F�C�v�̖��O����v������A��v�����V�F�C�v�̃A�h���X�Ɗ֐���hash�ŕR�Â���
	for (physx::PxU32 i = 0; i < shapeCount; ++i) {
		if (shapes[i]->getName() == triggerName) {
			gTriggerFuncs[shapes[i]] = triggerFunc;
		}
	}
	delete[] shapes;
}

//--------------------------------------
// �������E�Ɉʒu�𔽉f����
//--------------------------------------
void StaticRigidBody::ReflectPosToPhysXWorld() {
	physx::PxTransform nowTrans(physx::PxIdentity);
	nowTrans.p = physx::PxVec3(ToPxVec(mOwner.GetPos()));
	mRigidStatic->setGlobalPose(nowTrans);
}

//-----------------------------------
//  �������E�̈ʒu�𔽉f����
//-----------------------------------
void StaticRigidBody::ReflectPosOfPhysXWorld() {
	DirectX::XMFLOAT3 pos = ToXMFLOAT3(mRigidStatic->getGlobalPose().p);
	mOwner.AddPos(pos - mOwner.GetPos());
}

//----------------------------------------------------------------
// ���̍��̂Ƃ̂߂荞�݂𖳂������߂̃g���K�[�����t����֐�
//----------------------------------------------------------------
void DynamicRigidBody::AttachPreventingPenetrationBox(float hx,float hy,float hz,const DirectX::XMFLOAT3& localPos) {
	::AttachBoxTrigger(mRigidDynamic, hx, hy, hz, {}, "PreventingBox", localPos);
	// �߂荞�݂���������֐�
	auto preventingFunc = [this](const HitObjectInfo& info) {
		// Object�Ɠ��������玟�̃t���[���ŁA�O�̈ړ���������2.5�{���ɖ߂����Ƃł߂荞�݂���������
		if (info.triggerEvent == TriggerEvent::EEnter && info.name == "Object") {
			mNextAdditionalPos = 2.5 * (mOwner.GetPrevFramePos() - mOwner.GetPos());
		}
	};
	SetTriggerFunc(preventingFunc, "PreventingBox"); // �߂荞�݉����֐����g���K�[�ɐݒ�
}

//------------------------------------------------
// Dynamic���̂Ɍv������蓖�Ă�
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
//  triggerName�ƈ�v�����g���K�[�������ɓ����������ɌĂяo���֐���ݒ肷��
//-------------------------------------------------------------------------------
void DynamicRigidBody::SetTriggerFunc(std::function<void(const HitObjectInfo&)> triggerFunc, const std::string& triggerName) {
	physx::PxU32 shapeCount = mRigidDynamic->getNbShapes();    // mRigidBody�Ɋ��蓖�Ă�ꂽ�V�F�C�v�̐����擾
	physx::PxShape** shapes = new physx::PxShape * [shapeCount];
	mRigidDynamic->getShapes(shapes, shapeCount); // shapes��mRigidBody�Ɋ��蓖�Ă�ꂽ�V�F�C�v���擾
	// triggerName�ƃV�F�C�v�̖��O����v������A��v�����V�F�C�v�̃A�h���X�Ɗ֐���hash�ŕR�Â���
	for (physx::PxU32 i = 0; i < shapeCount; ++i) {
		if (shapes[i]->getName() == triggerName) {
			gTriggerFuncs[shapes[i]] = triggerFunc;
		}
	}
	delete[] shapes;
}

//-----------------------------------------
//  ���x����n�֐�
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
	// �������ւ̗͂��ق�0�Ȃ璅�n���Ă���Ƃ���
	if (Math::NearEqualF(mRigidDynamic->getLinearVelocity().y * 0.1f, 0.0f)) return true;
	return false;
}

//-----------------------------------------
// �������E�Ɉʒu�𔽉f����
//-----------------------------------------
void DynamicRigidBody::ReflectPosToPhysXWorld() {
	mPosBeforeUpdatePhysX = mOwner.GetPos();
	physx::PxTransform nowTrans(physx::PxIdentity);
	nowTrans.p = physx::PxVec3(ToPxVec(mOwner.GetPos()));
	mRigidDynamic->setGlobalPose(nowTrans);
}
//-----------------------------------------
// �������E�̈ʒu���Q�[���ɔ��f����
//-----------------------------------------
void DynamicRigidBody::ReflectPosOfPhysXWorld() {
	// physx��update������ɂɕω�����owner�̈ʒu�B������Ō�ɉ��Z���邱�ƂŁA�g���K�[�֐��ɂ��ʒu�ω��𔽉f����B
	DirectX::XMFLOAT3 deltaPosByTriggerFunc = mOwner.GetPos() - mPosBeforeUpdatePhysX;

	DirectX::XMFLOAT3 pos = ToXMFLOAT3(mRigidDynamic->getGlobalPose().p); // �������E�̈ʒu����
	DirectX::XMFLOAT3 additionalPos = pos - mOwner.GetPos();              // �Q�[�����E�̈ʒu�����������̂��A�Q�[�����E�ɉ��Z����ʒu�ω�
	// xz�����ւ̈ʒu�ω����������Ȃ�0�ɂ��邱�ƂŁA�u���b�N��[�ɏ�����Ƃ��Ɋ��藎����̂�������x�h���B
	if (Math::NearEqualF(additionalPos.x * 2.0f, 0.0f)) additionalPos.x = 0.0f;
	if (Math::NearEqualF(additionalPos.z * 2.0f, 0.0f)) additionalPos.z = 0.0f;
	mOwner.AddPos(additionalPos);         // ���Z���ĕ������E�̈ʒu���Q�[���ɔ��f����B

	mOwner.AddPos(deltaPosByTriggerFunc); // �g���K�[�֐��ɂ��ʒu�ω������Z����B
}



//-----------------------------
// PhysX�\���̂̊֐��Q
//-----------------------------
PhysX::PhysX() {

	// Foundation�̃C���X�^���X��
	if (mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocator, mDefaultErrorCallback); !mFoundation) {
		assert(!"PhysXFoundation���������s");
		return;
	}
	// PVD�Ɛڑ�����ݒ�
	if (mPvd = physx::PxCreatePvd(*mFoundation); mPvd) {
		// PVD���̃f�t�H���g�|�[�g��5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	}
	// Physics�̃C���X�^���X��
	if (mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true, mPvd); !mPhysics) {
		assert(!"PhyX���������s");
		return;
	}
	// �g���@�\�p
	if (!PxInitExtensions(*mPhysics, mPvd)) {
		assert(!"physX�g���@�\�ݒ莸�s");
		return;
	}
	// �����Ɏg���X���b�h���w�肷��
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
	// ��Ԃ̐ݒ�
	physx::PxSceneDesc scene_desc(mPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0.0f, -20.0f, 0.0f);
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	scene_desc.cpuDispatcher = mDispatcher;
	scene_desc.simulationEventCallback = new Trigger{};
	// ��Ԃ̃C���X�^���X��
	mPhysXScene = mPhysics->createScene(scene_desc);
	// PVD�̕\���ݒ�
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