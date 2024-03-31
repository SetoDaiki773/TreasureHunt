#pragma once
#include <vector>
#include <DirectXMath.h>
#include <memory>

//==========================================================
/// [機能] 数字を描画できる。
//==========================================================
class NumberUI {
public:
	NumberUI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale);
	NumberUI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale,int num);
	NumberUI(const NumberUI&) = default;

	//--------------------------------------------------------------------
	/// [機能] 第二引数で指定した位置に第三引数で指定した数値を描画する
	//--------------------------------------------------------------------
	void Render(const class Camera& camera,int num);

	//-----------------------------------------------------------
	// [機能] メンバ変数の数値を用いて描画する
	//-----------------------------------------------------------
	void Render(const class Camera& camera);

private:
	DirectX::XMFLOAT3 mPos;
	DirectX::XMFLOAT3 mScale;
	int mNum;
	static std::vector<class Model> mNumberUIs;  // 数字がかかれたテクスチャ軍 0～9まで
};
