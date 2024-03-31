#pragma once
#include <DirectXMath.h>

//----------------------------------------------------------------------------
/// ベクトルの計算
//----------------------------------------------------------------------------
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator+=(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator-=(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator*(float scaler, const DirectX::XMFLOAT3& vector);
const DirectX::XMFLOAT3 operator/(float scaler, const DirectX::XMFLOAT3& vector);
const DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& vector,float scaler);


namespace Vector {
	
	//-------------------------------------------
	/// [機能] 3次元ベクトルをコンソールに表示
	/// [引数] 3次元ベクトル
	//-------------------------------------------
	void Print(const DirectX::XMFLOAT3& vector);

	//---------------------------------------------
	/// [機能] ベクトルの長さの取得
	/// [引数] 3次元ベクトル
	/// [返り値] ベクトルの長さ
	//---------------------------------------------
	float GetLenght(const DirectX::XMFLOAT3& vector);

	//----------------------------------------------------------
	/// [機能] ベクトルの長さの二乗を取得。GetLenghtよりも速い
	/// [引数] 3次元ベクトル
	/// [返り値] ベクトルの長さの二乗
	//----------------------------------------------------------
	float GetLenghtSquare(const DirectX::XMFLOAT3& vector);

	//---------------------------------------------
	/// [機能] ベクトルの正規化
	/// [引数] 3次元ベクトル
	/// [返り値] 正規化されたベクトル
	//---------------------------------------------
	DirectX::XMFLOAT3 GetNormalize(const DirectX::XMFLOAT3& vector);


	//---------------------------------------------
	/// [機能] ベクトルの内積の取得
	/// [引数1] 3次元ベクトル1つ目
	/// [引数2] 3次元ベクトル2つ目
	/// [戻り値] ベクトルの内積
	//---------------------------------------------
	constexpr float GetDot(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);

	//----------------------------------------------
	/// [機能] 自身から目標物への角度を求める
	/// [引数1] 自分の位置ベクトル
	/// [引数2] 目標物の位置ベクトル
	/// [戻り値] 目標物のある角度
	//----------------------------------------------
	DirectX::XMFLOAT3 GetAngleToTarget(const DirectX::XMFLOAT3& ownVector, const DirectX::XMFLOAT3& targetVector);


	//---------------------------------------------
	/// [機能] XMVECTOR型をXMFLOAT3型に変換します
	/// [引数] 変換したいベクトルです
	/// [返り値] 変換されたXMFLOAT3です
	//---------------------------------------------
	DirectX::XMFLOAT3 ConvertVectorToFloat3(const DirectX::XMVECTOR& vector);
	DirectX::XMVECTOR ConvertFloat3ToVector(const DirectX::XMFLOAT3& float3);

}
