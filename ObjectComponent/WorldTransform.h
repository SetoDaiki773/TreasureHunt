#pragma once
#include <DirectXMath.h>
#include <memory>

//===================================================================
/// [機能] 位置、回転、拡大率の変更や取得などが出来るクラス。
//===================================================================
class WorldTransform {
public:
	
	//------------------------------------------------------------------------------
	/// [機能] コンストラクタ。引数を用いてワールドトランスフォームの初期化をします。位置、回転、拡大率を決定できます
	/// [引数1] 位置です
	/// [引数2] 回転角(度数法)です
	/// [引数3] 拡大率です
	//------------------------------------------------------------------------------
	explicit WorldTransform(
		const DirectX::XMFLOAT3& pos = DirectX::XMFLOAT3(0.0f,0.0f,0.0f),
		const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1.0f,1.0f,1.0f),
		const DirectX::XMVECTOR& quaternion = DirectX::XMQuaternionIdentity()
	);
	~WorldTransform();

	//----------------------------------------------------
	/// [機能] 位置をposにします
	/// [引数] 更新したい位置です
	//----------------------------------------------------
	void SetPos(const DirectX::XMFLOAT3& pos);

	//----------------------------------------------------
	/// [機能] 位置情報を取得します
	/// [返り値] 位置情報です
	//----------------------------------------------------
	const DirectX::XMFLOAT3& GetPos()const;

	//----------------------------------------------------
	/// [機能] 角度をrotに更新します
	/// [引数] 更新したい角度
	//----------------------------------------------------
	void SetRot(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------------------
	/// [機能] 角度情報を取得します
	/// [返り値] 角度情報です
	//----------------------------------------------------
	const DirectX::XMVECTOR& GetRot()const;

	//----------------------------------------------------
	/// [機能] 拡大率をscaleに変更します
	/// [引数] 更新したい拡大率です
	//----------------------------------------------------
	void SetScale(const DirectX::XMFLOAT3& scale);

	//----------------------------------------------------
	/// [機能] 拡大率を取得します
	/// [返り値] 拡大率です
	//----------------------------------------------------
	const DirectX::XMFLOAT3& GetScale()const;

	//-----------------------------------------------------
	/// [機能] 位置のベクトルを引数分プラスする
	/// [引数] 加えるベクトル
	//-----------------------------------------------------
	void AddPos(const DirectX::XMFLOAT3& position);

	//----------------------------------------------------
	/// [機能] 回転のベクトルを引数分プラスする
	/// [引数] 加えるベクトル
	//----------------------------------------------------
	void AddRot(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------------------
	/// [機能] 拡大率のベクトルを引数分プラスする
	/// [引数] 加えるベクトル
	//----------------------------------------------------
	void AddScale(const DirectX::XMFLOAT3& scale);

	//----------------------------------------------------
	/// [機能] ワールド行列の取得
	/// [返り値] ワールド行列
	//----------------------------------------------------
	const DirectX::XMMATRIX& GetWorldMat();

	//-----------------------------------------------------
	/// [機能] ワールド行列の更新
	//-----------------------------------------------------
	void UpdateWorldMat();

private:
	DirectX::XMFLOAT3 mPos;
	DirectX::XMVECTOR mQuaternion;
	DirectX::XMFLOAT3 mScale;
	DirectX::XMMATRIX mWorldMat;
	bool mWasChanged;
};

using WorldPtrU = std::unique_ptr<WorldTransform>;
using WorldPtrS = std::shared_ptr<WorldTransform>;