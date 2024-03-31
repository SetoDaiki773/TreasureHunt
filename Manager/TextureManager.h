#pragma once
#include "Rendering/ComPtr.h"
#include <string>
#include <d3d12.h>
#include <unordered_map>
#include <memory>

class Texture {
public:
	unsigned GetHeapOffset();
	friend class TextureManager;
private:
	std::wstring name;   // �e�N�X�`���[�t�@�C����
	DXGI_FORMAT format;  // �e�N�X�`���t�H�[�}�b�g
	ComPtr<ID3D12Resource> resource;  // �e�N�X�`�����\�[�X
	std::unique_ptr<class HeapOffset> heapOffset; // �q�[�v�I�t�Z�b�g
};

using TexturePtr = std::shared_ptr<Texture>;
using Textures = std::unordered_map<std::wstring, TexturePtr>;


//========================================================================================
/// [�@�\] �����̃e�N�X�`�����܂Ƃ߂ĊǗ��B�g���܂킷�e�N�X�`��������Ȃ�A�������g��
//========================================================================================
class TextureManager {
public:
	//---------------------------------------------
	/// [�@�\] ���ʕ����̏������B��x�����Ăяo��
	//---------------------------------------------
	static void Init(HWND* hWnd);
	TextureManager();

	//-----------------------------------------------------
	/// [�@�\] �������Ɠ����t�@�C���p�X�̃e�N�X�`����ǉ�
	//-----------------------------------------------------
	void AddTexture(const std::wstring& textureName);

	//---------------------------------------------------
	/// [�@�\] �����Ɠ����t�@�C���p�X�̃e�N�X�`�����擾
	//---------------------------------------------------
	TexturePtr GetTexture(const std::wstring& textureName);

	//---------------------------------------------------
	/// [�@�\] �����Ɠ����t�@�C���p�X�̃e�N�X�`�����폜
	//---------------------------------------------------
	void RemoveTexture(const std::wstring& textureName);

private:
	Textures mTextures;
	static std::shared_ptr<class Renderer> mRenderer;
};
