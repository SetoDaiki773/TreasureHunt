#pragma once
#include <d3d12.h>
#include <string>
#include "ComPtr.h"
#include <d3dx12.h>
#include <memory>

//===========================================================================================
/// [機能] シェーダ名でパイプラインステートを作る。aブレンドは有効にしている。
//===========================================================================================
class PipelineState {
public:
	PipelineState(const std::wstring& vertexShaderFileName,const std::wstring& pixelShaderName);
	explicit PipelineState(const std::wstring& shaderFileName);

	//----------------------------------------------------
	/// [機能] DirectX形式のパイプラインステートを取得
	//----------------------------------------------------
	ID3D12PipelineState* GetPipelineState() { return mPipelineState.Get(); };
	static ID3D12RootSignature* GetRootSignature() { return mRootSignature.Get(); };

private:
	void Init(const std::wstring& vertexShaderFileName,const std::wstring& pixelShaderFileName);
	void CreateDefaultPipelineState();
	void CreateRootSignature();
	ComPtr<ID3D12PipelineState> mPipelineState;
	static ComPtr<ID3D12RootSignature> mRootSignature;
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC mDefaultPipelineStateDesc;
};
