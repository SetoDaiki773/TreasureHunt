#include "GameObject.h"
#include <iostream>
#include <Windows.h>
#include "ObjectComponent/Model.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Component.h"
#include "Common/MyMath.h"
#include "Camera/Camera.h"


using namespace DirectX;


//----------------------------------------------------------------
//      �R���X�g���N�^�[(�ʒu�A��]�A�g�嗦�A������)�̏�����
//----------------------------------------------------------------
GameObject::GameObject(
	ModelPtrS resource,
	WorldTransform&& world,
	int expectedNumComponents
	)
	:mResource(resource)
	,mState(State::EActive)
	,mWorld(std::make_unique<WorldTransform>(std::move(world)))
	,mPrevFramePos(mWorld->GetPos())
	,mComponents()
{
	mComponents.reserve(expectedNumComponents);
}




//-------------------------------------
//  �I�u�W�F�N�g�X�V
//-------------------------------------
void GameObject::Update() {
	mPrevFramePos = mWorld->GetPos();
	if (!(mState == State::EActive)) return;
	for (auto comp : mComponents) {
		comp.second->Update();
	}
	
}


//-------------------------------------------------
//    3D�I�u�W�F�N�g�̕`��
//-------------------------------------------------
void GameObject::Render(const Camera& camera)
{
	if (mState == State::EUnDraw || mState == State::EStopUndraw) return;
	mWorld->UpdateWorldMat();
	ProjectionComponents projComp = camera.GetProjectionComponents();
	//if (!VFCulling(mWorld->GetPos(),camera.GetViewMat(),projComp.fov,projComp.nearClip,projComp.farClip,projComp.aspect)) {
	//	return;
	//}

	mResource->Render(mWorld->GetWorldMat(), camera);
}


//--------------------------------------------------
//   �e���}�b�v����
//--------------------------------------------------
void GameObject::ShadowMap(const Camera& camera) {
	if (mState != State::EActive) return;
	mWorld->UpdateWorldMat();
	//mModel->ShadowMap(mWorld->GetWorldMat(),camera);
}


//---------------------------------------
//   �G�x���g���󂯎��
//---------------------------------------
void GameObject::ReceiveEvent(const Event& event) {

}

//---------------------------------------
//  �I�u�W�F�N�g�������Ă悢�̂��擾
//---------------------------------------
bool GameObject::NeedErase()const {
	if(mState == State::EDead) return true;
	return false;
	
}

//-------------------------------------------------
//   ���[���h�g�����X�t�H�[���̃Q�b�^�[�Z�b�^�[
//-------------------------------------------------
void GameObject::SetWorld(const WorldTransform& world) {
	*mWorld = world;
}

const WorldTransform& GameObject::GetWorld()const {
	return *mWorld.get();
}

void GameObject::SetPos(const XMFLOAT3& pos) {
	mWorld->SetPos(pos);
}

const XMFLOAT3& GameObject::GetPos()const {
	return mWorld->GetPos();
}

void GameObject::AddPos(const XMFLOAT3& pos) {
	mWorld->AddPos(pos);
}

void GameObject::SetQuaternion(const DirectX::XMVECTOR& quaternion) {
	mWorld->SetRot(quaternion);
}

DirectX::XMVECTOR GameObject::GetQuaternion()const {
	return mWorld->GetRot();
}

void GameObject::SetScale(const DirectX::XMFLOAT3& scale) {
	mWorld->SetScale(scale);
}

DirectX::XMFLOAT3 GameObject::GetScale()const {
	return mWorld->GetScale();
}


void GameObject::SetState(GameObject::State state) {
	mState = state;
}

GameObject::State GameObject::GetState() {
	return mState;
}

DirectX::XMFLOAT3 GameObject::GetPrevFramePos() {
	return mPrevFramePos;
}

