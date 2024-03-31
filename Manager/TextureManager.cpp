#include "TextureManager.h"
#include "Scene/Sequence.h"
#include <iostream>
#include <cassert>
#include "Common/FileUtil.h"
#include "Rendering/Renderer.h"
#include "Rendering/HeapOffset.h"
#include "ObjectComponent/Model.h"


std::shared_ptr<Renderer> TextureManager::mRenderer;

//-------------------------------------------
//   �e�N�X�`���t�@�C���̃t�H�[�}�b�g�𓾂�
//-------------------------------------------
namespace {
	DXGI_FORMAT GetTextureFileFormat(const std::wstring& textureFileName) {
		std::string extension = ToUTF8(FileExtension(textureFileName)); // ���̂����C�h�����񂾂ƕ������r�����܂������Ȃ��̂�string�ɂ���
		if (extension == "png") {
			return DXGI_FORMAT_B8G8R8A8_UNORM;   // png�t�@�C���̃t�H�[�}�b�g
		}
		else if (extension == "jpg") {
			return DXGI_FORMAT_R8G8B8A8_UNORM;   // jpg�t�@�C���̃t�H�[�}�b�g
		}
		else if (extension == "bmp") {
			return DXGI_FORMAT_R8G8B8A8_UNORM;   // bmp�t�@�C���̃t�H�[�}�b�g
		}
		else {
			assert(!"����Ή����ĂȂ��g���q");
		}
		return DXGI_FORMAT_UNKNOWN;  // �R���p�C���̌x�����邢����K���ɕԂ��Ƃ��Ă�A�ǂ���assert�����
	}
}

void TextureManager::Init(HWND* hWnd) {
	static bool isFirst = true;
	if (isFirst) {
		mRenderer = std::make_shared<Renderer>(hWnd);
		Model::Init(mRenderer);
		isFirst = false;
	}
}

TextureManager::TextureManager() {
	
}

unsigned Texture::GetHeapOffset() {
	return heapOffset->GetHeapOffset();
}

void TextureManager::AddTexture(const std::wstring& textureName) {
	auto iter = mTextures.find(textureName);
	if (iter != mTextures.end()) {  
		return; // ���Ƃ��Ǝ����Ă���e�N�X�`���������炻�̂܂܋A��
	}

	// �����ĂȂ������������
#ifdef _DEBUG
	/*if (Sequence::IsRunTime()) {
		std::cout << "RunTime���̃��f�����[�h : " << ToUTF8(textureName) << "\n";
		assert(!"�����^�C�����ɐV�����e�N�X�`�������[�h���Ȃ���!!");
	}*/
#endif
	TexturePtr texture = std::make_shared<Texture>();
	texture->name = textureName;
	texture->format = GetTextureFileFormat(textureName);
	texture->heapOffset = std::make_unique<HeapOffset>();
	
	mRenderer->CreateTexture(texture->resource.Get(), texture->name, texture->format, texture->heapOffset->GetHeapOffset());
	mTextures.emplace(textureName, texture);
}

TexturePtr TextureManager::GetTexture(const std::wstring& textureName) {
	auto iter = mTextures.find(textureName);
	if (iter != mTextures.end()) {
		return iter->second;  // �����Ă����炻���Ԃ�
	}
	AddTexture(textureName);  // �����ĂȂ�����������Ă���Ԃ�
	return mTextures[textureName];
}

void TextureManager::RemoveTexture(const std::wstring& textureName) {
	mTextures.erase(textureName);
}
