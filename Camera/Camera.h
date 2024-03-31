#pragma once
#include <DirectXmath.h>
#include <memory>


struct ProjectionComponents {
	float nearClip;
	float farClip;
	float fov;
	float aspect;
};

//======================================================
/// [機能] ゲームワールドのカメラの役割をするクラスです
//======================================================
class Camera {
public:
	

	Camera(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& lookPos,std::unique_ptr<class CameraMovement> cameraMovement);
	

	//-------------------------------------
	/// [機能]カメラの状態の更新
	//-------------------------------------
	void Update();
	

	//------------------------------------------
	/// [機能] ビュー行列の取得
	/// [返り値] ビュー行列
	//------------------------------------------
	const DirectX::XMMATRIX& GetViewMat()const { return mViewMat; }

	//------------------------------------------
	/// [機能] プロジェクション行列の取得
	/// [返り値] プロジェクション行列
	//------------------------------------------
	const DirectX::XMMATRIX& GetProjMat()const { return mProjMat; }

	//-----------------------------------------
	/// [機能] カメラの位置を取得します
	/// [返り値] カメラの位置です
	//-----------------------------------------
	DirectX::XMFLOAT3 GetPos()const { return mPosition; }

	DirectX::XMFLOAT3 GetLookPos()const { return mLookPos; }

	//----------------------------------------------
	/// [機能] プロジェクション行列の構成要素の取得
	/// [返り値] プロジェクション行列の構成要素
	//-----------------------------------------------
	ProjectionComponents GetProjectionComponents()const;

	void ChangeMovement(class CameraMovement* cameraMovement);

private:
	//-----------------------------------------
	/// [機能] ビュー行列の更新
	//-----------------------------------------
	void UpdateViewMat();

	//-----------------------------------------
	/// [機能] プロジェクション行列の更新
	//-----------------------------------------
	void UpdateProjMat();


	std::unique_ptr<class CameraMovement> mMovement;
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mLookPos;
	DirectX::XMMATRIX mViewMat;
	DirectX::XMMATRIX mProjMat;
	float mFov;
	float mNearClip;
	float mFarClip;
	float mAspect;               
};

typedef std::unique_ptr<Camera> CameraPtrU;
