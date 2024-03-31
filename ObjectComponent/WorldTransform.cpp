#include "WorldTransform.h"
#include "Common/VectorOperation.h"


using namespace DirectX;

WorldTransform::WorldTransform(
	const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& scale,
	const DirectX::XMVECTOR& quaternion
	)
	:mPos(pos)
	,mQuaternion(quaternion)
	,mScale(scale)
	,mWasChanged(true)
	,mWorldMat()
	
{

	UpdateWorldMat();
	
}

WorldTransform::~WorldTransform() {

}

//-------------------------------------------------
// 位置のセッター、ゲッター
//-------------------------------------------------
void WorldTransform::SetPos(const XMFLOAT3& pos) {
	mPos = pos;
	mWasChanged = true;
}

const XMFLOAT3& WorldTransform::GetPos()const {
	return mPos;
}

//------------------------------------------------
//   角度のセッター、ゲッター
//------------------------------------------------
void WorldTransform::SetRot(const XMVECTOR& rot) {
	mQuaternion = rot;
	mWasChanged = true;
}

const XMVECTOR& WorldTransform::GetRot()const {
	return mQuaternion;
}

//------------------------------------------------
//   拡大率のセッター、ゲッター
//------------------------------------------------
void WorldTransform::SetScale(const XMFLOAT3& scale) {
	mScale = scale;
	mWasChanged = true;
}

const XMFLOAT3& WorldTransform::GetScale()const {
	return mScale;
}

//------------------------------------------------
//   位置ベクトルにベクトルを足す
//------------------------------------------------
void WorldTransform::AddPos(const XMFLOAT3& pos) {
	mPos = mPos + pos;
	mWasChanged = true;
}


//------------------------------------------------
//   回転ベクトルにベクトルを足す
//------------------------------------------------
void WorldTransform::AddRot(const XMVECTOR& quaternion) {
	mQuaternion = XMQuaternionMultiply(quaternion,mQuaternion);
	mWasChanged = true;
}

//------------------------------------------------
//   拡大率ベクトルにベクトルを足す
//------------------------------------------------
void WorldTransform::AddScale(const XMFLOAT3& scale) {
	mScale = mScale + scale;
	mWasChanged = true;
}



//----------------------------------------------
//   ワールド行列の取得
//----------------------------------------------
const DirectX::XMMATRIX& WorldTransform::GetWorldMat() {
	return mWorldMat;
}


//-----------------------------------------------
//   ワールド行列の更新
//-----------------------------------------------
void WorldTransform::UpdateWorldMat() {
	if (!mWasChanged) return;

	// ワールド座標の更新用の行列の準備
	XMMATRIX scaleMat;
	scaleMat = XMMatrixScaling(mScale.x, mScale.y, mScale.z);   // スケール行列
	XMMATRIX quaternionMat;
	quaternionMat = XMMatrixRotationQuaternion(mQuaternion);    // 回転行列
	XMMATRIX translation;
	translation = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);  // 位置行列

		
	// ワールド座標更新
	mWorldMat = XMMatrixIdentity();
	mWorldMat = scaleMat * quaternionMat * translation;
	mWasChanged = false;
}
