#pragma once
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

//--------------------------------------------------------------------------
/// [機能]　第一引数の位置にいるオブジェクトが画面内にいるのかを判定します
//--------------------------------------------------------------------------
bool VFCulling(const DirectX::XMFLOAT3& pos, const DirectX::XMMATRIX& pmView, float Angle, float NearClip, float FarClip, float Aspect); // 視錐台

enum class Shape {
	Cube,
	Square
};


//=========================================================================================================================
/// [機能] モデルとは1つ以上のメッシュの塊である、とこのプログラム内では定義する。それを描画したり、テクスチャを変えたりできる
//=========================================================================================================================
class Model{
public:
	static void Init(std::shared_ptr<class Renderer> renderer);

	//----------------------------------------------------------------
	/// [機能] 3Dモデルファイルをもとにモデルを形成する
	///        頂点シェーダとピクセルシェーダが別ファイルの時に使う
	//----------------------------------------------------------------
	Model(const wchar_t* modelFileName,const std::wstring& vertexShaderName,const std::wstring& pixelSaderName);
	//-----------------------------------------------------------------
	/// [機能] 3Dモデルファイルをもとにモデルを形成する
	///        頂点シェーダとピクセルシェーダが同一ファイルの時に使う
	//-----------------------------------------------------------------
	Model(const wchar_t* modelFileName,const std::wstring& shaderName);
	//------------------------------------------------------------------
	/// [機能] Shape列挙型の形状のモデルを形成する。
	///        頂点シェーダとピクセルシェーダが別ファイルの時に使う
	//------------------------------------------------------------------
	Model(Shape shape, const std::wstring& textureName,const std::wstring& vertexShaderName,const std::wstring& pixelShaderName);
	//-------------------------------------------------------------------
	/// [機能] Shape列挙型の形状のモデルを形成する
	///        頂点シェーダとピクセルシェーダが同一ファイルの時に使う
	//-------------------------------------------------------------------
	Model(Shape shape, const std::wstring& textureName,const std::wstring& shaderName);


	~Model();

	Model(Model&&);
	Model& operator=(Model&&);

	
	//-------------------------------------------------------
	/// [機能] そのフレームでの描画開始時に一度だけ呼び出す
	//-------------------------------------------------------
	static void BeginRender();

	//-----------------------------------------------------------------
	// [機能] そのフレームでの描画が全て終わったときに一度だけ呼び出す
	//-----------------------------------------------------------------
	static void EndRender();

	//-----------------------------------------------------
	/// [機能] ワールドマトリクスとカメラ位置をもとに描画
	//-----------------------------------------------------
	void Render(const DirectX::XMMATRIX& worldMat,const class Camera& camera);

	//--------------------------------------------------------------------------------------------------------------------
	/// [機能] 影情報をテクスチャに書き込む。
	/// [注意] 影を描画するわけではない。全ての影をテクスチャに書き込んだのちにレンダーを呼び出すと正しく影が書かれる
	//--------------------------------------------------------------------------------------------------------------------
	void ShadowMap(const DirectX::XMMATRIX& worldMat,const class Camera& camera);

	//---------------------------------------------------------------
	/// [機能] このモデルが複数のメッシュから構成されているか調べる
	//---------------------------------------------------------------
	bool ConsistsMultipleMesh();

	//-------------------------------------------------------------
	/// [機能] テクスチャを変更する。
	/// [引数1] テクスチャ名
	/// [引数2] モデルを構成する、何番目のメッシュを使用するのか
	//-------------------------------------------------------------
	void ChangeTexture(const std::wstring& textureName,int meshIndex);

	//-------------------------------------------------------------
	/// [機能] 0番目のメッシュのテクスチャを変更する。
	/// [引数1] テクスチャ名
	//-------------------------------------------------------------
	void ChangeTexture(const std::wstring& textureName);

private:
	std::vector<class MyMesh> mMeshes;  // モデルのメッシュ
	std::shared_ptr<class PipelineState> mPipelineState;
	static std::unique_ptr<class PipelineManager> mPipelineManager;
	static std::shared_ptr<class Renderer> mRenderer;
	static bool mWasInstanced;

	// コピーは重いので封印しておく
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;
};



using ModelPtrS = std::shared_ptr<Model>;

