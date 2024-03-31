#pragma once
#include <DirectXmath.h>

namespace Quaternion {


	//---------------------------------------------
	/// [機能] 前方ベクトルをクォータニオンで取得します
	/// [引数] クォータニオンです
	/// [返り値] 前方ベクトルです
	//---------------------------------------------
	DirectX::XMFLOAT3 GetFront(const DirectX::XMVECTOR& quaternion);


	//----------------------------------------
	/// [機能] 後方ベクトルを取得します
	/// [引数] クォータニオンです
	/// [返り値] 後方ベクトルです
	//----------------------------------------
	DirectX::XMFLOAT3 GetBack(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------
	/// [機能] 右ベクトルを取得します
	/// [引数] クォータニオンです
	/// [返り値] 右ベクトルです
	//----------------------------------------
	DirectX::XMFLOAT3 GetRight(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------
	/// [機能] 左ベクトルを取得します
	/// [引数] クォータニオンです
	/// [返り値] 左ベクトルです
	//----------------------------------------
	DirectX::XMFLOAT3 GetLeft(const DirectX::XMVECTOR& quaternion);


	//-----------------------------------------------
	/// [機能] 度数法で回転四元数を取得します
	//-----------------------------------------------
	DirectX::XMVECTOR RotateEuler(float pitch, float yaw, float roll);
	DirectX::XMVECTOR RotateAxis(const DirectX::XMVECTOR& axis, float degreeAngle);


	//-------------------------------------------------------------------
	/// [機能] オイラー角で回転させ、ソースのクォータニオンと乗算します
	/// [引数1,2,3] ロール、ピッチ、ヨー其ぞれの回転量です(度数法) 
	/// [引数4] 算出されたクォータニオンと乗算したいクォータニオンです
	/// [返り値] 乗算結果です
	//--------------------------------------------------------------------
	DirectX::XMVECTOR RotateEulerAndMultiply(float pitch, float yaw, float roll, const DirectX::XMVECTOR& srcQuaternion);

	//-----------------------------------------------------------------
	/// [機能] 任意軸で回転させ、ソースのクォータニオンと乗算します
	/// [引数1] 回転軸です
	/// [引数2] 回転量(度数法)
	/// [引数2] 算出されたクォータニオンと乗算したいクォータニオンです
	/// [返り値] 乗算結果です
	//-----------------------------------------------------------------
	DirectX::XMVECTOR RotateAxisAndMultiply(const DirectX::XMVECTOR& axis, float degreeAngle, const DirectX::XMVECTOR& srcQuaternion);


}  // end Quaternion namespace
