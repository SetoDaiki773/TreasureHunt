#pragma once
#include <windows.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "ComPtr.h"
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

const int noanoganuganougnoaungajntjanjganj = 1;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"Gdi32.lib") 
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"DirectXTK12.lib") 

//コンスタントバッファ
struct Cbuffer
{
	DirectX::XMMATRIX w;          // ワールド行列
	DirectX::XMMATRIX wvp;        // ワールドから射影までの変換行列
	DirectX::XMFLOAT4 eyePos;     // 視点位
	DirectX::XMFLOAT4 ambient;    // 環境光
	DirectX::XMFLOAT4 diffuse;    // 拡散反射光
	DirectX::XMFLOAT4 specular;   // 鏡面反射光
	DirectX::XMFLOAT4 lightDir;   // ライト方向
	Cbuffer()
	{
		ZeroMemory(this, sizeof(Cbuffer));
	}
};

//========================================================================================
/// [機能] メッシュなどの描画クラス。描画処理は最終的にすべてこのクラスで実行される
//========================================================================================
class Renderer {
public:
	
	void BeginRender();
	void EndRender();

	Renderer(HWND* hWnd);
	void CreateTexture(ID3D12Resource* resource, std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset);   // テクスチャを作る
	
	//----------------------------------------
	/// [機能] 影情報をテクスチャに書き込む
	//----------------------------------------
	void ShadowMap(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);

	//--------------------------------------------
	/// [機能] オブジェクトを描画する
	//--------------------------------------------
	void RenderObject(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh,class PipelineState* shader);

private:
	// インスタンスは一つにしたいので封印
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	HRESULT CreateRenderObject(HWND* hWnd);
	void WaitGpu();        // GPUとの同期

	void Render(const DirectX::XMMATRIX& worldMat, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat, const DirectX::XMFLOAT3& eyeVec, const class MyMesh& mesh);

	
	// 描画インターフェース
	UINT64 mFenceValue;                                // フェンス値
	UINT mRTVStride;                                   // レンダ―ターゲットビュー一つ当たりの大きさ
	UINT mCBV_SRV_Stride;                              // レンダ―ターゲットビューとコンスタントバッファ―ビュー一つ当たりの大きさ
	UINT mCBSize;                                      // コンスタントバッファ―一つ当たりの大きさ
	static const int FrameCount = 2;                          // フレイムの数(キャンバスの数?)
	UINT* mCBVDataBegin;
	ComPtr<IDXGISwapChain3> mSwapChain;                // スワップチェーン
	ComPtr<ID3D12Device> mDevice;                      // デバイス
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;  // コマンドアロケータ
	ComPtr<ID3D12CommandQueue> mCommandQueue;          // コマンドキュー
	ComPtr<ID3D12GraphicsCommandList> mCommandList;    // コマンドリスト
	ComPtr<ID3D12Fence> mFence;                        // フェンス
	unsigned mShadowTextureHeapOffset;                 // シャドウマップ用テクスチャのヒープ位置
	ComPtr<ID3D12RootSignature> mRootSignature;        // ルートシグネチャ

	// パイプラインステート
	ComPtr<ID3D12PipelineState> mPipelineWriteShadow;  // シャドウマップ書き込み用パイプライン
	ComPtr<ID3D12PipelineState> mPipelineReadShadow;   // シャドウマップ読み込み用


	// CBV SRV系
	ComPtr<ID3D12Resource> mConstantBuffer;            // コンスタントバッファ
	ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;      // コンスタント、シェダーリソース用のヒープ

	// RTV DSV系
	ComPtr<ID3D12Resource> mRenderTarget[FrameCount];  // レンダ―ターゲットのリソース
	ComPtr<ID3D12DescriptorHeap> mRTVHeap;             // レンダ―ターゲットのヒープ
	ComPtr<ID3D12Resource> mDepthBuffer;               // 深度バッファ
	ComPtr<ID3D12DescriptorHeap> mDSPHeap;             // 深度バッファのヒープ
	// シャドウマップのためのRTV DSP
	ComPtr<ID3D12Resource> mShadowRTVTexture;          // シャドウマップのテクスチャレンダ―ターゲット
	ComPtr<ID3D12DescriptorHeap> mShadowRTVHeap;       // シャドウマップのレンダ―ターゲットのヒープ
	ComPtr<ID3D12Resource> mShadowDepthBuffer;         // シャドウマップの深度バッファ
	ComPtr<ID3D12DescriptorHeap> mShadowDSPHeap;       // シャドウマップの深度バッファヒープ

	std::vector<class HeapOffset> mCbvHeapOffsets;     // コンスタントバッファビューのヒープオフセット
	bool mIsFullScreenMode = false;                    // スクリーンモード

	const unsigned mNumMaxModels = 100;                // モデルの最大数
	const unsigned mNumMaxMaterials = 10;              // モデル一つあたりの最大マテリアル数
	const unsigned mNumMaxModelTypes = 20;             // モデルの種類の最大数
	bool mBeganRender = false;
	bool mEndedRender = false;
};


typedef std::unique_ptr<Renderer> RendererPtrU;