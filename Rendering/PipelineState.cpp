#include "PipelineState.h"
#include "Device.h"
#include <iostream>
#include <D3Dcompiler.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"d3dcompiler.lib")

D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineState::mDefaultPipelineStateDesc;
ComPtr<ID3D12RootSignature> PipelineState::mRootSignature;

PipelineState::PipelineState(const std::wstring& vertexShaderFileName,const std::wstring& pixelShaderFileName) 
	:mPipelineState()
{
	Init(vertexShaderFileName,pixelShaderFileName);
}

PipelineState::PipelineState(const std::wstring& shaderFileName) {
	Init(shaderFileName,shaderFileName);
}


void PipelineState::Init(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName) {
	static bool isFirstInstance = true;
	if (isFirstInstance) {
		CreateDefaultPipelineState();
		isFirstInstance = false;
	}

	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	D3DCompileFromFile(vertexShaderFileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShaderBlob, nullptr);
	D3DCompileFromFile(pixelShaderFileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShaderBlob, nullptr);

	D3D12_INPUT_ELEMENT_DESC inputElement[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};


	// パイプラインステート作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = mDefaultPipelineStateDesc;
	psDesc.InputLayout = { inputElement,_countof(inputElement) };
	psDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob); // デフォルトのパイプラインステートの頂点シェーダと
	psDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob);  // ピクセルシェーダだけ変更する

	Device::GetDevice()->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(mPipelineState.GetAddressOf()));
}


void PipelineState::CreateDefaultPipelineState() {
	
	CreateRootSignature();

	
	//ブレンド用の設定
	D3D12_RENDER_TARGET_BLEND_DESC bdesc = {};
	bdesc.BlendEnable = true;
	bdesc.LogicOpEnable = false;
	bdesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	bdesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	bdesc.BlendOp = D3D12_BLEND_OP_ADD;
	bdesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	bdesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	bdesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	bdesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	bdesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	
	// パイプラインステート作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& psDesc = mDefaultPipelineStateDesc;
	mDefaultPipelineStateDesc.pRootSignature = mRootSignature.Get();
	mDefaultPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	mDefaultPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	mDefaultPipelineStateDesc.RasterizerState.FrontCounterClockwise = false;
	mDefaultPipelineStateDesc.BlendState.RenderTarget[0] = bdesc;
	mDefaultPipelineStateDesc.DepthStencilState.DepthEnable = true;
	mDefaultPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	mDefaultPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	mDefaultPipelineStateDesc.DepthStencilState.FrontFace = defaultStencilOp;
	mDefaultPipelineStateDesc.DepthStencilState.BackFace = defaultStencilOp;
	mDefaultPipelineStateDesc.DepthStencilState.StencilEnable = false;
	mDefaultPipelineStateDesc.SampleMask = UINT_MAX;
	mDefaultPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	mDefaultPipelineStateDesc.NumRenderTargets = 1;
	mDefaultPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	mDefaultPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	mDefaultPipelineStateDesc.SampleDesc.Count = 1;
	

}

void PipelineState::CreateRootSignature() {
	
	// ルートシグネチャの作成(シェーダにリソースの位置を教える)
	// サンプラーを作成
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	// なんかレンジの指定の時のやつがあるからシグネチャに直接入れられるのかな?
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// レンジを作ってそれでルートパラメータをつくる
	// 関連付ける必要のあるリソースのタイプが二つなのでルートパラメータとレンジは二つ
	// コンスタントバッファビューとシェダーリソースビュー
	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	// 関連付けるリソースがコンスタントバッファビューなので第一引数はTYPE_CBV
	// 第3引数は一番小さいレジスター番号
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	// 関連付ける二つ目はシェダーリソースビュー
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	// 三つめもシェダーリソースビュー
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_ALL);


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	ID3DBlob* error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	
	Device::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf()));

}