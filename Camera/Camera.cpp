#include "Camera.h"
#include "Main.h"
#include "Common/MyMath.h"
#include "CameraMovement.h"

Camera::Camera(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& lookPos,std::unique_ptr<CameraMovement> cameraMovement)
	:mMovement(std::move(cameraMovement))
	,mPosition(cameraPos)
	, mLookPos(lookPos)
	, mViewMat()
	, mProjMat()
	, mFov(DirectX::XM_PI / 4)
	, mNearClip(0.3f)
	, mFarClip(1000.0f)
	, mAspect(static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight))
{
	UpdateViewMat();
	UpdateProjMat();
}

//--------------------------------------
//  カメラの更新
//--------------------------------------
void Camera::Update() {
	mMovement->Excute(mPosition,mLookPos);
	UpdateViewMat();
}

void Camera::ChangeMovement(CameraMovement* cameraMovement) {
	mMovement.reset(cameraMovement);
}


//--------------------------------------
//  View行列の更新
//--------------------------------------
void Camera::UpdateViewMat() {
	DirectX::XMFLOAT3 pos = mPosition;   // カメラ位置の取得
	DirectX::XMFLOAT3 look = mLookPos;   // 注視位置の取得
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0.0f);    // XMFLOAT3型をXMVECTORに変換
	DirectX::XMVECTOR lookPos = DirectX::XMVectorSet(look.x, look.y, look.z, 0.0f);   // XMFLOAT3型をXMVECTORに変換
	DirectX::XMVECTOR upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);       //カメラの上方ベクトル
	mViewMat = DirectX::XMMatrixLookAtLH(eyePos, lookPos, upward);   // ビュー行列の設定
}

//--------------------------------------
//  Projection行列の更新
//--------------------------------------
void Camera::UpdateProjMat() {
	
	mProjMat = DirectX::XMMatrixIdentity();
	mProjMat = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspect, mNearClip, mFarClip);

}


//------------------------------------------
//  Projection行列の構成要素の構造体を取得
//------------------------------------------
ProjectionComponents Camera::GetProjectionComponents()const {
	ProjectionComponents projComp;
	projComp.farClip = mFarClip;
	projComp.nearClip = mNearClip;
	projComp.fov = mFov;
	projComp.aspect = mAspect;
	return projComp;
}