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

//�R���X�^���g�o�b�t�@
struct Cbuffer
{
	DirectX::XMMATRIX w;          // ���[���h�s��
	DirectX::XMMATRIX wvp;        // ���[���h����ˉe�܂ł̕ϊ��s��
	DirectX::XMFLOAT4 eyePos;     // ���_��
	DirectX::XMFLOAT4 ambient;    // ����
	DirectX::XMFLOAT4 diffuse;    // �g�U���ˌ�
	DirectX::XMFLOAT4 specular;   // ���ʔ��ˌ�
	DirectX::XMFLOAT4 lightDir;   // ���C�g����
	Cbuffer()
	{
		ZeroMemory(this, sizeof(Cbuffer));
	}
};

//========================================================================================
/// [�@�\] ���b�V���Ȃǂ̕`��N���X�B�`�揈���͍ŏI�I�ɂ��ׂĂ��̃N���X�Ŏ��s�����
//========================================================================================
class Renderer {
public:
	
	void BeginRender();
	void EndRender();

	Renderer(HWND* hWnd);
	void CreateTexture(ID3D12Resource* resource, std::wstring textureName,DXGI_FORMAT fileFormat,unsigned heapOffset);   // �e�N�X�`�������
	
	//----------------------------------------
	/// [�@�\] �e�����e�N�X�`���ɏ�������
	//----------------------------------------
	void ShadowMap(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh);

	//--------------------------------------------
	/// [�@�\] �I�u�W�F�N�g��`�悷��
	//--------------------------------------------
	void RenderObject(const DirectX::XMMATRIX& worldMat, const class Camera& camera, const class MyMesh& mesh,class PipelineState* shader);

private:
	// �C���X�^���X�͈�ɂ������̂ŕ���
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	HRESULT CreateRenderObject(HWND* hWnd);
	void WaitGpu();        // GPU�Ƃ̓���

	void Render(const DirectX::XMMATRIX& worldMat, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat, const DirectX::XMFLOAT3& eyeVec, const class MyMesh& mesh);

	
	// �`��C���^�[�t�F�[�X
	UINT64 mFenceValue;                                // �t�F���X�l
	UINT mRTVStride;                                   // �����_�\�^�[�Q�b�g�r���[�������̑傫��
	UINT mCBV_SRV_Stride;                              // �����_�\�^�[�Q�b�g�r���[�ƃR���X�^���g�o�b�t�@�\�r���[�������̑傫��
	UINT mCBSize;                                      // �R���X�^���g�o�b�t�@�\�������̑傫��
	static const int FrameCount = 2;                          // �t���C���̐�(�L�����o�X�̐�?)
	UINT* mCBVDataBegin;
	ComPtr<IDXGISwapChain3> mSwapChain;                // �X���b�v�`�F�[��
	ComPtr<ID3D12Device> mDevice;                      // �f�o�C�X
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;  // �R�}���h�A���P�[�^
	ComPtr<ID3D12CommandQueue> mCommandQueue;          // �R�}���h�L���[
	ComPtr<ID3D12GraphicsCommandList> mCommandList;    // �R�}���h���X�g
	ComPtr<ID3D12Fence> mFence;                        // �t�F���X
	unsigned mShadowTextureHeapOffset;                 // �V���h�E�}�b�v�p�e�N�X�`���̃q�[�v�ʒu
	ComPtr<ID3D12RootSignature> mRootSignature;        // ���[�g�V�O�l�`��

	// �p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> mPipelineWriteShadow;  // �V���h�E�}�b�v�������ݗp�p�C�v���C��
	ComPtr<ID3D12PipelineState> mPipelineReadShadow;   // �V���h�E�}�b�v�ǂݍ��ݗp


	// CBV SRV�n
	ComPtr<ID3D12Resource> mConstantBuffer;            // �R���X�^���g�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;      // �R���X�^���g�A�V�F�_�[���\�[�X�p�̃q�[�v

	// RTV DSV�n
	ComPtr<ID3D12Resource> mRenderTarget[FrameCount];  // �����_�\�^�[�Q�b�g�̃��\�[�X
	ComPtr<ID3D12DescriptorHeap> mRTVHeap;             // �����_�\�^�[�Q�b�g�̃q�[�v
	ComPtr<ID3D12Resource> mDepthBuffer;               // �[�x�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> mDSPHeap;             // �[�x�o�b�t�@�̃q�[�v
	// �V���h�E�}�b�v�̂��߂�RTV DSP
	ComPtr<ID3D12Resource> mShadowRTVTexture;          // �V���h�E�}�b�v�̃e�N�X�`�������_�\�^�[�Q�b�g
	ComPtr<ID3D12DescriptorHeap> mShadowRTVHeap;       // �V���h�E�}�b�v�̃����_�\�^�[�Q�b�g�̃q�[�v
	ComPtr<ID3D12Resource> mShadowDepthBuffer;         // �V���h�E�}�b�v�̐[�x�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> mShadowDSPHeap;       // �V���h�E�}�b�v�̐[�x�o�b�t�@�q�[�v

	std::vector<class HeapOffset> mCbvHeapOffsets;     // �R���X�^���g�o�b�t�@�r���[�̃q�[�v�I�t�Z�b�g
	bool mIsFullScreenMode = false;                    // �X�N���[�����[�h

	const unsigned mNumMaxModels = 100;                // ���f���̍ő吔
	const unsigned mNumMaxMaterials = 10;              // ���f���������̍ő�}�e���A����
	const unsigned mNumMaxModelTypes = 20;             // ���f���̎�ނ̍ő吔
	bool mBeganRender = false;
	bool mEndedRender = false;
};


typedef std::unique_ptr<Renderer> RendererPtrU;