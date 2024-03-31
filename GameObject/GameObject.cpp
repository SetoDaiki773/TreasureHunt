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
//      コンストラクター(位置、回転、拡大率、見た目)の初期化
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
//  オブジェクト更新
//-------------------------------------
void GameObject::Update() {
	mPrevFramePos = mWorld->GetPos();
	if (!(mState == State::EActive)) return;
	for (auto comp : mComponents) {
		comp.second->Update();
	}
	
}


//-------------------------------------------------
//    3Dオブジェクトの描画
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
//   影をマップする
//--------------------------------------------------
void GameObject::ShadowMap(const Camera& camera) {
	if (mState != State::EActive) return;
	mWorld->UpdateWorldMat();
	//mModel->ShadowMap(mWorld->GetWorldMat(),camera);
}


//---------------------------------------
//   エベントを受け取る
//---------------------------------------
void GameObject::ReceiveEvent(const Event& event) {

}

//---------------------------------------
//  オブジェクトを消してよいのか取得
//---------------------------------------
bool GameObject::NeedErase()const {
	if(mState == State::EDead) return true;
	return false;
	
}

//-------------------------------------------------
//   ワールドトランスフォームのゲッターセッター
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

