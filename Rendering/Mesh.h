#pragma once
#include <dxgi1_4.h>
#include "ComPtr.h"
#include <windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <memory>
#include <string>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"DirectXTK12.lib") 


// マテリアル構造体
struct MyMaterial
{
	MyMaterial();
	DirectX::XMFLOAT4 ambient;        // アンビエント
	DirectX::XMFLOAT4 diffuse;        // ディフューズ
	DirectX::XMFLOAT4 specular;       // スペキュラー
	std::shared_ptr<class Texture> texture;
	bool hasTexture;                  // このマテリアルがテクスチャを持っているのか
};

//============================================================
/// [機能] Mesh...つまりポリゴンの塊を作って、管理するクラス。
//============================================================
class  MyMesh {

public:
	//----------------------------------------------------------------------------------
	/// [機能] いくつかのmeshのロード、1モデルに複数のmeshを含む可能性があるのでコンテナ
	/// [引数1] デバイスアドレス
	/// [引数2] ロードするファイル名
	/// [引数3] meshコンテナ 
	//-----------------------------------------------------------------------------------
	friend void LoadMeshesFromFile(ID3D12Device* pDevice, const wchar_t* FileName, std::vector<class MyMesh>& mesh);

private:
	friend HRESULT LoadMesh(ID3D12Device* device,class MyMesh& dst, const struct aiMesh* src);
	
public:
	MyMesh();
	~MyMesh();

	MyMesh(MyMesh&&) = default;

	//-------------------------------------------------
	/// [機能] 四角形メッシュを作る
	/// [引数1] 四角形に張り付けるテクスチャ名
	//-------------------------------------------------
	void CreateSquareMesh(const std::wstring& textureName);

	//------------------------------------------------
	/// [機能]  立方体をつくる
	/// [引数1] 立方体に張り付けるテクスチャ名
	//------------------------------------------------
	void CreateCubeMesh(const std::wstring& textureName);

	//-----------------------------------------------
	/// [機能] 引数のテクスチャにテクスチャを変える
	//-----------------------------------------------
	void ChangeTexture(const std::wstring& textureName);


	const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView()const;
	const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView()const;
	MyMaterial GetMaterial()const;
	DWORD GetNumTriangles()const;
	unsigned GetSrvHeapOffset()const;

private:
	


	DWORD mNumTriangles;                              // 三角形数
	//std::unique_ptr<class HeapOffset> mSrvHeapOffset; // シェダーリソースのヒープオフセット
	ComPtr<ID3D12Resource> mVertexBuffer;             // 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;       // 頂点バッファビュー
	ComPtr<ID3D12Resource> mIndexBuffer;              // インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;         // インデックスバッファビュー
	MyMaterial mMaterial;                             // マテリアル
	static std::unique_ptr<class TextureManager> mTextureManager;
	
};


//頂点の構造体
struct MyVertex
{
	DirectX::XMFLOAT3 pos;     // 位置
	DirectX::XMFLOAT3 normal;  // 法線
	DirectX::XMFLOAT2 uv;      // uv

};

