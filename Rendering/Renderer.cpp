#include "Renderer.h"
#include "Main.h"
#include "Common/FileUtil.h"
#include "Mesh.h"
#include "Camera/Camera.h"
#include "HeapOffset.h"
#include "PipelineState.h"
#include "Device.h"
#include <cassert>
#include <sstream>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace DirectX;



HRESULT Renderer::CreateRenderObject(HWND* hWnd) {
	assert(hWnd != nullptr || !"������HWND��null�|�C���^");
	
	UINT dxgiFactoryFlags = 0;

	// DirectX�̃f�o�b�O�֗��c�[��
#if defined(_DEBUG)	
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
	// �f�o�C�X�̐���
	mDevice = Device::GetDevice();

	// �e���\�[�X�r���[�������̑傫�������߂�
	mRTVStride = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mCBV_SRV_Stride = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �R�}���h�L���[�쐬(�R�}���h���X�g�����s����̂͂�����)
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf()));

	// �R�}���h���X�g�쐬(�`��R�}���h�����߂Ĉ�C�Ɏ��s���č�������}��炵��)
	mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()));
	mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), NULL, IID_PPV_ARGS(mCommandList.GetAddressOf()));
	mCommandList->Close();

	// �X���b�v�`�F�[���쐬(�t���[���̊Ǘ�������)
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = WindowWidth;
	swapChainDesc.Height = WindowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	IDXGIFactory4* factory;
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	factory->CreateSwapChainForHwnd(mCommandQueue.Get(), *hWnd, &swapChainDesc, NULL, NULL, (IDXGISwapChain1**)mSwapChain.GetAddressOf());   // �����œ����Ń����_�\�^�[�Q�b�g�̃o�b�t�@�������
	if (mIsFullScreenMode) {
		mSwapChain->SetFullscreenState(true, NULL);
		mSwapChain->ResizeBuffers(FrameCount, WindowWidth, WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	}
	// �����_�\�^�[�Q�b�g�̍쐬(�`���)
	// �����_�\�^�[�Q�b�g�̃q�[�v���܂����
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRTVHeap.GetAddressOf()));

	// ���Ƀ����_�\�^�[�Q�b�g�r���[�����
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());             // �q�[�v�̃A�h���X�����蓖�Ăăq�[�v�̒��Ƀr���[�����������悤�ɂ��Ă���
	for (UINT n = 0; n < FrameCount; n++) {
		mSwapChain->GetBuffer(n, IID_PPV_ARGS(mRenderTarget[n].GetAddressOf()));                         // �X���b�v�`�F�[���̒��Ƀo�b�t�@������̂ł��炤
		mDevice->CreateRenderTargetView(mRenderTarget[n].Get(), NULL, rtvHandle);                        // ��������NULL�����K�v�Ȃ�\���̂�����ċ@�\��ς���
		rtvHandle.Offset(1, mRTVStride);   // ��̃����_�\�^�[�Q�b�g�r���[�̑傫������������i�߂�
	}

	//�[�x�o�b�t�@�̍쐬
	D3D12_HEAP_PROPERTIES dsHeapProp = {};
	ZeroMemory(&dsHeapProp, sizeof(&dsHeapProp));
	dsHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	dsHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	dsHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	dsHeapProp.CreationNodeMask = NULL;
	dsHeapProp.VisibleNodeMask = NULL;

	D3D12_RESOURCE_DESC dsResDesc = {};
	ZeroMemory(&dsResDesc, sizeof(D3D12_RESOURCE_DESC));
	dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsResDesc.Alignment = 0;
	dsResDesc.Width = WindowWidth;
	dsResDesc.Height = WindowHeight;
	dsResDesc.DepthOrArraySize = 1;
	dsResDesc.MipLevels = 1;
	dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsResDesc.SampleDesc.Count = 1;
	dsResDesc.SampleDesc.Quality = 0;
	dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue;
	ZeroMemory(&clearValue, sizeof(D3D12_CLEAR_VALUE));
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	mDevice->CreateCommittedResource(
		&dsHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&dsResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(mDepthBuffer.GetAddressOf())
	);

	// �[�x�o�b�t�@�̃r���[�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	ZeroMemory(&dsvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = NULL;
	mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDSPHeap.GetAddressOf()));

	// �[�x�o�b�t�@�r���[
	D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	ZeroMemory(&dsViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsViewDesc.Texture2D.MipSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsViewDesc, dsvHandle);


	// �t�F���X���쐬(GPU��CPU�̓������Ƃ邽�߂̂��)
	mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));
	mFenceValue = 1;

	// �R���X�^���g�o�b�t�@�r���[�ƃV�F�_�[���\�[�X�r���[�p�̃q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC csHeapDesc = {};
	unsigned numConstantBuffers = mNumMaxMaterials * mNumMaxModels;  // �\�z�����R���X�^���g�o�b�t�@�̐�
	unsigned numTextures = mNumMaxMaterials * mNumMaxModelTypes;     // �\�z�����e�N�X�`���̐�
	unsigned numDescriptors = numConstantBuffers + numTextures;      // �\�z�����f�B�X�N���v�^�̐�
	csHeapDesc.NumDescriptors = numDescriptors;
	csHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	csHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	mDevice->CreateDescriptorHeap(&csHeapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));

	// �R���X�^���g�o�b�t�@�̍쐬(��{�I�Ƀ��[���h�}�b�g�Ƃ��̃V�F�[�_�ň�����������ꏊ)
	// 256�̔{���ɃT�C�Y���A���C�����g����
	mCBSize = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT * ((sizeof(Cbuffer) / D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) + 1);
	D3D12_HEAP_PROPERTIES cHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC cRDesc = CD3DX12_RESOURCE_DESC::Buffer(mCBSize * numConstantBuffers);
	HRESULT hr = mDevice->CreateCommittedResource(
		&cHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cRDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mConstantBuffer.GetAddressOf())
	);
	if (!SUCCEEDED(hr)) {
		assert(!"�R���X�^���g�o�b�t�@�������s");
	}
	// �R���X�^���g�o�b�t�@�̐������A�R���X�^���g�o�b�t�@�r���[�����
	for (DWORD i = 0; i < numConstantBuffers; i++) {
		// �r���[�����
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		// �o�b�t�@�̈ʒu��CB�T�C�Y�ł��炷
		cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + (i * mCBSize);
		cbvDesc.SizeInBytes = mCBSize;
		D3D12_CPU_DESCRIPTOR_HANDLE cHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cHandle.ptr += i * mCBV_SRV_Stride; // �n���h���̈ʒu�̓r���[�̃X�g���C�h�ł��炷
		mDevice->CreateConstantBufferView(&cbvDesc, cHandle);
	}

	// �R���X�^���g�o�b�t�@�̃}�b�v�A�e�t���[���ł�mCBVDataBegin���n�_�Ƃ��āA�ϊ����Ȃǂ���������ł���(�e�t���[����memcpy�����ł悢)
	CD3DX12_RANGE readRange(0, 0);
	readRange = CD3DX12_RANGE(0, 0);
	mConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mCBVDataBegin));

	// �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g
	D3D12_RESOURCE_DESC shadowResDesc = {};
	ZeroMemory(&shadowResDesc, sizeof(shadowResDesc));
	shadowResDesc.MipLevels = 1;
	shadowResDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shadowResDesc.Width = WindowWidth;
	shadowResDesc.Height = WindowHeight;
	shadowResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	shadowResDesc.DepthOrArraySize = 1;
	shadowResDesc.SampleDesc.Count = 1;
	shadowResDesc.SampleDesc.Quality = 0;
	shadowResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE clearValueShadowRTV = {};
	ZeroMemory(&clearValueShadowRTV, sizeof(D3D12_CLEAR_VALUE));
	clearValueShadowRTV.Format = DXGI_FORMAT_R32_FLOAT;
	clearValueShadowRTV.Color[0] = 0.0f;
	clearValueShadowRTV.Color[1] = 0.0f;
	clearValueShadowRTV.Color[2] = 0.5f;
	clearValueShadowRTV.Color[3] = 1.0f;
	clearValueShadowRTV.DepthStencil.Depth = 1.0f;
	clearValueShadowRTV.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES shadowRTVHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	mDevice->CreateCommittedResource(
		&shadowRTVHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&shadowResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&clearValueShadowRTV,
		IID_PPV_ARGS(mShadowRTVTexture.GetAddressOf())
	);

	// �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC shadowResHeapDesc = {};
	shadowResHeapDesc.NumDescriptors = FrameCount;
	shadowResHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	shadowResHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDevice->CreateDescriptorHeap(&shadowResHeapDesc, IID_PPV_ARGS(mShadowRTVHeap.GetAddressOf()));
	// �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g�r���[
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart());
	mDevice->CreateRenderTargetView(mShadowRTVTexture.Get(), NULL, shadowRTVHandle);

	// �V���h�E�}�b�v�p�̃V�F�_�[���\�[�X�r���[�����
	D3D12_SHADER_RESOURCE_VIEW_DESC shadowSRDesc = {};
	shadowSRDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shadowSRDesc.Format = shadowResDesc.Format;
	shadowSRDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	shadowSRDesc.Texture2D.MipLevels = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	mShadowTextureHeapOffset = numDescriptors - 1;
	handle.ptr += mShadowTextureHeapOffset * mCBV_SRV_Stride;
	mDevice->CreateShaderResourceView(mShadowRTVTexture.Get(), &shadowSRDesc, handle);

	// �V���h�E�}�b�v�p�̐[�x�o�b�t�@
	D3D12_HEAP_PROPERTIES shadowDepthHeapProp = {};
	ZeroMemory(&shadowDepthHeapProp, sizeof(&shadowDepthHeapProp));
	shadowDepthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	shadowDepthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	shadowDepthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	shadowDepthHeapProp.CreationNodeMask = NULL;
	shadowDepthHeapProp.VisibleNodeMask = NULL;

	D3D12_RESOURCE_DESC shadowDepthResDesc = {};
	ZeroMemory(&shadowDepthResDesc, sizeof(D3D12_RESOURCE_DESC));
	shadowDepthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	shadowDepthResDesc.Alignment = 0;
	shadowDepthResDesc.Width = WindowWidth;
	shadowDepthResDesc.Height = WindowHeight;
	shadowDepthResDesc.DepthOrArraySize = 1;
	shadowDepthResDesc.MipLevels = 1;
	shadowDepthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDepthResDesc.SampleDesc.Count = 1;
	shadowDepthResDesc.SampleDesc.Quality = 0;
	shadowDepthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	shadowDepthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValueShadowDSP = {};
	ZeroMemory(&clearValueShadowDSP, sizeof(D3D12_CLEAR_VALUE));
	clearValueShadowDSP.Format = DXGI_FORMAT_D32_FLOAT;
	clearValueShadowDSP.DepthStencil.Depth = 1.0f;
	clearValueShadowDSP.DepthStencil.Stencil = 0;
	mDevice->CreateCommittedResource(
		&shadowDepthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&shadowDepthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValueShadowDSP,
		IID_PPV_ARGS(mShadowDepthBuffer.GetAddressOf())
	);

	// �V���h�E�}�b�v�[�x�o�b�t�@�r���[�̃q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC shadowDSVHeapDesc = {};
	ZeroMemory(&shadowDSVHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	shadowDSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	shadowDSVHeapDesc.NumDescriptors = 1;
	shadowDSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	shadowDSVHeapDesc.NodeMask = NULL;
	mDevice->CreateDescriptorHeap(&shadowDSVHeapDesc, IID_PPV_ARGS(mShadowDSPHeap.GetAddressOf()));

	// �V���h�E�}�b�v�[�x�o�b�t�@�r���[
	D3D12_DEPTH_STENCIL_VIEW_DESC shadowDSViewDesc = {};
	ZeroMemory(&shadowDSViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	shadowDSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	shadowDSViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	shadowDSViewDesc.Texture2D.MipSlice = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateDepthStencilView(mShadowDepthBuffer.Get(), &shadowDSViewDesc, shadowDSVHandle);
	/*
	// �V���h�E�}�b�v�p
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescWriteShadow = psDesc;
	psDescWriteShadow.DepthStencilState.DepthEnable = true;
	psDescWriteShadow.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDescWriteShadow.VS = CD3DX12_SHADER_BYTECODE(writeShadowVertexShader);
	psDescWriteShadow.PS = CD3DX12_SHADER_BYTECODE(writeShadowPixelShader);
	psDescWriteShadow.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
	mDevice->CreateGraphicsPipelineState(&psDescWriteShadow, IID_PPV_ARGS(mPipelineWriteShadow.GetAddressOf()));
	// �V���h�E�}�b�v�ǂݍ��ݗp
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDescReadShadow = psDescWriteShadow;
	psDescReadShadow.DepthStencilState.DepthEnable = true;
	psDescReadShadow.VS = CD3DX12_SHADER_BYTECODE(readShadowVertexShader);
	psDescReadShadow.PS = CD3DX12_SHADER_BYTECODE(readShadowPixelShader);
	psDescReadShadow.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	mDevice->CreateGraphicsPipelineState(&psDescReadShadow, IID_PPV_ARGS(mPipelineReadShadow.GetAddressOf()));
	*/
	
	return S_OK;
}

//---------------------------------
// ���̃t���[���ł̕`��J�n
//---------------------------------
void Renderer::BeginRender() {
	assert(!mBeganRender);
	mBeganRender = true;

	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	//�R�}���h���X�g�ƃA���P�[�^�����Z�b�g���Ƃ�
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), mPipelineWriteShadow.Get());


	// �S�Ă̕`��ŋ��ʂ̃R�}���h���R�}���h���X�g�ɂ��߂Ă���


	//���\�[�X�o���A�̏�Ԃ������_�\�^�[�Q�b�g���[�h�ɕύX
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mRenderTarget[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier(1, &barrier);

	//�r���[�|�[�g���Z�b�g(�`��̈�)
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight));
	CD3DX12_RECT scissorRect = CD3DX12_RECT(0, 0, WindowWidth, WindowHeight);
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);


	// �V���h�E�p���[�_�[�^�[�Q�b�g�n���h���̎擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	// �V���h�E�p�����_�\�^�[�Q�b�g�̃Z�b�g
	// mCommandList->OMSetRenderTargets(1, &shadowRTVHandle, false, &shadowDSVHandle); �����ŃZ�b�g����K�v�͂Ȃ�

	// �V���h�E�}�b�v�p�����_�\�^�[�Q�b�g�̃N���A
	const float clearColor[] = { 0.5f,0.5f,0.5f,1.0f };
	mCommandList->ClearRenderTargetView(shadowRTVHandle, clearColor, 0, NULL);
	mCommandList->ClearDepthStencilView(shadowDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	//mCommandList->SetPipelineState(mPipelineState.Get());


	// �J�����g�o�b�t�@�������_�\�^�[�Q�b�g�ɂ��� 
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();

	// ��ʃN���A
	mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
	mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// ���[�g�V�O�l�`�����Z�b�g
	mCommandList->SetGraphicsRootSignature(PipelineState::GetRootSignature());

	// �q�[�v���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { mDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);


	// �|���S���̌`����w��
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // �O�p�`���X�g

}

//-------------------------------
//  ���̃t���[���ł̕`�悪�I��
//-------------------------------
void Renderer::EndRender() {

	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = {};
	// ���\�[�X�o���A��Present���[�h�ɕύX
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mRenderTarget[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier(1, &barrier);

	// ���ߏI�����̂ŕ���
	mCommandList->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̐؂�ւ�
	mSwapChain->Present(1, 0);

	// GPU�Ɠ���
	WaitGpu();

	// �R���X�^���g�o�b�t�@�Ŏg�����q�[�v��������Ă���
	for (auto& heapOffset : mCbvHeapOffsets) {
		heapOffset.Release();
	}
	mCbvHeapOffsets.clear();
	mBeganRender = false;
}

Renderer::Renderer(HWND* hWnd) 
{
	static bool isFirstInstance = true;
	assert(isFirstInstance);  // �C���X�^���X��P��ɐ�������
	if (CreateRenderObject(hWnd) == E_FAIL) {
		assert(!"RendererObject�̐����Ɏ��s");
	}
	isFirstInstance = false;
}


//-----------------------------------
// �F�X������
//-----------------------------------
void Renderer::ShadowMap(const DirectX::XMMATRIX& worldMat, const Camera& camera,const MyMesh& mesh) {

	assert(mBeganRender);
	//World View Projection �ϊ�
	XMMATRIX viewMat = camera.GetViewMat();
	XMMATRIX projMat = camera.GetProjMat();
	XMFLOAT3 eyeVec = camera.GetPos();


	mCommandList->SetPipelineState(mPipelineWriteShadow.Get());
	// �V���h�E�p���[�_�[�^�[�Q�b�g�n���h���̎擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE shadowRTVHandle(mShadowRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE shadowDSVHandle = mShadowDSPHeap->GetCPUDescriptorHandleForHeapStart();
	// �V���h�E�p�����_�\�^�[�Q�b�g�̃Z�b�g
	mCommandList->OMSetRenderTargets(1, &shadowRTVHandle, false, &shadowDSVHandle);
	Render(worldMat, viewMat, projMat, eyeVec, mesh);
}



void Renderer::RenderObject(const XMMATRIX& worldMat, const Camera& camera,const MyMesh& mesh,PipelineState* pState) {

	assert(mBeganRender); // �`��J�n�֐���ǂ�ł��Ȃ��Ȃ�assert
	//World View Projection �ϊ�
	XMMATRIX viewMat = camera.GetViewMat();
	XMMATRIX projMat = camera.GetProjMat();
	XMFLOAT3 eyeVec = camera.GetPos();
	
	mCommandList->SetPipelineState(pState->GetPipelineState());
	

	// �J�����g�o�b�t�@�������_�\�^�[�Q�b�g�ɂ��� 
	// ���݂̃o�b�t�@(�`���)���擾
	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, mRTVStride);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSPHeap->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	Render(worldMat, viewMat, projMat, eyeVec, mesh);

}




//--------------------------------------------------------------------
//  �e�N�X�`���o�b�t�@�����A�����ǂ�����Ă�̂�����ܗ����ł��ĂȂ�
//--------------------------------------------------------------------
void Renderer::CreateTexture(ID3D12Resource* resource,std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset) {
	// �e�N�X�`���֘A����
	// �e�N�X�`�����������Ȃ��΂�
	if (textureName.size() <= 2) {
		assert(!"�e�N�X�`�����������B���̃��b�V���ɂ̓e�N�X�`�����܂܂�Ȃ��Ȃǂ̌������l�����܂�");
	}

	// �t�@�C������e�N�Z����T�C�Y����ǂݏo��
	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subRes;
	
#ifdef _DEBUG
	std::stringstream str;
	str << ToUTF8(textureName);
	
#endif
	//�e�N�Z����T�C�Y�����[�h
	if (!SUCCEEDED(LoadWICTextureFromFile(mDevice.Get(), textureName.c_str(), &resource, decodedData, subRes))) {
		assert(!str.str().c_str());
	}

	// �󔒂̃e�N�X�`��
	D3D12_RESOURCE_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(tDesc));
	tDesc.MipLevels = 1;
	tDesc.Format = fileFormat;
	tDesc.Width = subRes.RowPitch / 4;
	tDesc.Height = subRes.SlicePitch / subRes.RowPitch;
	tDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	tDesc.DepthOrArraySize = 1;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//�q�[�v�͓ǂݏ����ł��Ȃ�������DEFAULT
	D3D12_HEAP_PROPERTIES tHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	mDevice->CreateCommittedResource(
		&tHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&tDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	// CPU����GPU�Ƀe�N�X�`���f�[�^��n���Ƃ��̒��p�p�e�N�X�`�������
	ID3D12Resource* textureUploadHeap;
	// �q�[�v��CPU����ǂݏ����ł���UPLOAD
	D3D12_HEAP_PROPERTIES stHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// �f�[�^���A�b�v���[�h���邽�߂ɕK�v�ȑ傫�������߂�
	UINT64 bufferSize = GetRequiredIntermediateSize(resource, 0, 1);
	D3D12_RESOURCE_DESC stResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	mDevice->CreateCommittedResource(
		&stHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&stResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureUploadHeap)
	);

	// �R�}���h���X�g���g�����烊�Z�b�g���Ƃ�
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), 0);

	// �ǂ݂������e�N�Z���f�[�^���󔒂̃e�N�X�`���[�ɗ������݁A����������
	// �R�}���h���X�g�Ƀ��\�[�X���M���߂�����
	UpdateSubresources(mCommandList.Get(), resource, textureUploadHeap, 0, 0, 1, &subRes);
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	mCommandList->ResourceBarrier(1, &barrier);

	// �e�N�X�`���̑��M�����邽�߂ɃR�}���X�����
	mCommandList->Close();

	// �R�}���h���X�g���s
	ID3D12CommandList* commandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	WaitGpu();

	// �V�F�_�[���\�[�X�r���[����
	D3D12_SHADER_RESOURCE_VIEW_DESC sDesc = {};
	sDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	sDesc.Format = fileFormat;
	sDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	sDesc.Texture2D.MipLevels = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += heapOffset * mCBV_SRV_Stride;
	mDevice->CreateShaderResourceView(resource, &sDesc, handle);
	
}

//------------------------------
//  �`��{��
//------------------------------
void Renderer::Render(const XMMATRIX& worldMat, const XMMATRIX& viewMat, const XMMATRIX& projMat, const XMFLOAT3& eyeVec,const MyMesh& mesh ) {
	
	const MyMaterial& material = mesh.GetMaterial();
	// ���_�o�b�t�@���Z�b�g
	mCommandList->IASetVertexBuffers(0, 1, mesh.GetVertexBufferView());

	// �R���X�^���g�o�b�t�@�̓��e�̍X�V
	Cbuffer cb;   // �������ޓ��e
	char* ptr = reinterpret_cast<char*>(mCBVDataBegin); // �������ݐ�A�h���X�A
	HeapOffset cbvHeapOffset;
	// �o�b�t�@�̂ǂ��ɏ������ނ��𒲐�����
	ptr += mCBSize * cbvHeapOffset.GetHeapOffset();
	// �R���X�^���g�o�b�t�@�ݒ�
	// ���C�g�r���[(�V���h�E�}�b�v��������Ƃ��Ɏg��)
	/*XMMATRIX lightViewMat = XMMatrixIdentity();
	XMMATRIX lightTranslation = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
	XMMATRIX lightRotation = XMMatrixRotationX(XMConvertToRadians(90.0f));

	lightViewMat = lightRotation * lightTranslation;
	lightViewMat = XMMatrixInverse(nullptr, lightViewMat);*/

	//�������ޓ��e�𐶐�
	cb.wvp = XMMatrixTranspose(worldMat * viewMat * projMat); // hlsl��DirectX�ōs�D���D�悪�قȂ�̂œ]�u
	cb.w = XMMatrixTranspose(worldMat);
	cb.lightDir = XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f);
	cb.eyePos = XMFLOAT4(eyeVec.x, eyeVec.y, eyeVec.z, 1.0f);
	cb.ambient = XMFLOAT4(material.ambient.x, material.ambient.y,
		material.ambient.z, material.ambient.w);
	cb.diffuse = XMFLOAT4(material.diffuse.x, material.diffuse.y,
		material.diffuse.z, material.diffuse.w);
	cb.specular = XMFLOAT4(material.specular.x, material.specular.y,
		material.specular.z, material.specular.w);
	
	// UV�ϊ��s��(�V���h�E�}�b�v��������Ƃ��Ɏg��)
	/*XMFLOAT4 f1(0.5f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vec1 = XMLoadFloat4(&f1);
	XMFLOAT4 f2(0.0f, -0.5f, 0.0f, 0.0f);
	XMVECTOR vec2 = XMLoadFloat4(&f2);
	XMFLOAT4 f3(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR vec3 = XMLoadFloat4(&f3);
	XMFLOAT4 f4(0.5f, 0.5f, 0.0f, 1.0f);
	XMVECTOR vec4 = XMLoadFloat4(&f4);
	XMMATRIX clipMat(vec1, vec2, vec3, vec4);
	cb.wlpt = XMMatrixTranspose(worldMat * lightViewMat * projMat * clipMat);
	*/
	std::memcpy(ptr, &cb, sizeof(Cbuffer)); // ������R���X�^���g�o�b�t�@�̓��e��ptr�ɏ�������

	// CBV�Z�b�g
	// CBV�쐬���ɏ������悤�Ƀq�[�v�̍ŏ���CBV�������Ă�
	D3D12_GPU_DESCRIPTOR_HANDLE handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();


	// �f�B�X�N���v�^�q�[�v�̂ǂ�������̂����q�[�v�I�t�Z�b�g�Œ��߂���
	handle.ptr += cbvHeapOffset.GetHeapOffset() * mCBV_SRV_Stride; // �f�B�X�N���v�^�q�[�v�̈ʒu�Ȃ̂ŃX�g���C�h
	mCommandList->SetGraphicsRootDescriptorTable(0, handle);

	// ����������������ƃq�[�v�I�t�Z�b�g���������Ă��܂��̂ŕ`�悪�I���܂ŕۑ����Ă���
	mCbvHeapOffsets.push_back(cbvHeapOffset);


	// SRV�Z�b�g
	handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += mesh.GetSrvHeapOffset() * mCBV_SRV_Stride;
	mCommandList->SetGraphicsRootDescriptorTable(1, handle);

	// �C���f�b�N�X�o�b�t�@���Z�b�g
	mCommandList->IASetIndexBuffer(mesh.GetIndexBufferView());

	// �`��
	// �������͒��_���A�|���S������3�{
	mCommandList->DrawIndexedInstanced(mesh.GetNumTriangles() * 3, 1, 0, 0, 0);
}



//------------------------------------------
//  CPU��GPU�̓���������
//------------------------------------------
void Renderer::WaitGpu() {
	// GPU�̎d�����I������Ƃ���GPU����A���Ă���l���Z�b�g����
	mCommandQueue->Signal(mFence.Get(), mFenceValue);

	do {
		// �ҋ@
		// �Ȃ񂩂ł���悤�ɂ��Ȃ���?
	} while (mFence->GetCompletedValue() < mFenceValue); // �������I����Ă�Ɗ֐��̖߂�l�������ă��[�v�E�o����
	mFenceValue++;
}
