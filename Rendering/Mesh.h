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


// �}�e���A���\����
struct MyMaterial
{
	MyMaterial();
	DirectX::XMFLOAT4 ambient;        // �A���r�G���g
	DirectX::XMFLOAT4 diffuse;        // �f�B�t���[�Y
	DirectX::XMFLOAT4 specular;       // �X�y�L�����[
	std::shared_ptr<class Texture> texture;
	bool hasTexture;                  // ���̃}�e���A�����e�N�X�`���������Ă���̂�
};

//============================================================
/// [�@�\] Mesh...�܂�|���S���̉������āA�Ǘ�����N���X�B
//============================================================
class  MyMesh {

public:
	//----------------------------------------------------------------------------------
	/// [�@�\] ��������mesh�̃��[�h�A1���f���ɕ�����mesh���܂މ\��������̂ŃR���e�i
	/// [����1] �f�o�C�X�A�h���X
	/// [����2] ���[�h����t�@�C����
	/// [����3] mesh�R���e�i 
	//-----------------------------------------------------------------------------------
	friend void LoadMeshesFromFile(ID3D12Device* pDevice, const wchar_t* FileName, std::vector<class MyMesh>& mesh);

private:
	friend HRESULT LoadMesh(ID3D12Device* device,class MyMesh& dst, const struct aiMesh* src);
	
public:
	MyMesh();
	~MyMesh();

	MyMesh(MyMesh&&) = default;

	//-------------------------------------------------
	/// [�@�\] �l�p�`���b�V�������
	/// [����1] �l�p�`�ɒ���t����e�N�X�`����
	//-------------------------------------------------
	void CreateSquareMesh(const std::wstring& textureName);

	//------------------------------------------------
	/// [�@�\]  �����̂�����
	/// [����1] �����̂ɒ���t����e�N�X�`����
	//------------------------------------------------
	void CreateCubeMesh(const std::wstring& textureName);

	//-----------------------------------------------
	/// [�@�\] �����̃e�N�X�`���Ƀe�N�X�`����ς���
	//-----------------------------------------------
	void ChangeTexture(const std::wstring& textureName);


	const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView()const;
	const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView()const;
	MyMaterial GetMaterial()const;
	DWORD GetNumTriangles()const;
	unsigned GetSrvHeapOffset()const;

private:
	


	DWORD mNumTriangles;                              // �O�p�`��
	//std::unique_ptr<class HeapOffset> mSrvHeapOffset; // �V�F�_�[���\�[�X�̃q�[�v�I�t�Z�b�g
	ComPtr<ID3D12Resource> mVertexBuffer;             // ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;       // ���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> mIndexBuffer;              // �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;         // �C���f�b�N�X�o�b�t�@�r���[
	MyMaterial mMaterial;                             // �}�e���A��
	static std::unique_ptr<class TextureManager> mTextureManager;
	
};


//���_�̍\����
struct MyVertex
{
	DirectX::XMFLOAT3 pos;     // �ʒu
	DirectX::XMFLOAT3 normal;  // �@��
	DirectX::XMFLOAT2 uv;      // uv

};

