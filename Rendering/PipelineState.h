#pragma once
#include <d3d12.h>
#include <string>
#include "ComPtr.h"
#include <d3dx12.h>
#include <memory>

//===========================================================================================
/// [�@�\] �V�F�[�_���Ńp�C�v���C���X�e�[�g�����Ba�u�����h�͗L���ɂ��Ă���B
//===========================================================================================
class PipelineState {
public:
	PipelineState(const std::wstring& vertexShaderFileName,const std::wstring& pixelShaderName);
	explicit PipelineState(const std::wstring& shaderFileName);

	//----------------------------------------------------
	/// [�@�\] DirectX�`���̃p�C�v���C���X�e�[�g���擾
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
