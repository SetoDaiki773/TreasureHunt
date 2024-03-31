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
//   テクスチャファイルのフォーマットを得る
//-------------------------------------------
namespace {
	DXGI_FORMAT GetTextureFileFormat(const std::wstring& textureFileName) {
		std::string extension = ToUTF8(FileExtension(textureFileName)); // 何故かワイド文字列だと文字列比較がうまくいかないのでstringにする
		if (extension == "png") {
			return DXGI_FORMAT_B8G8R8A8_UNORM;   // pngファイルのフォーマット
		}
		else if (extension == "jpg") {
			return DXGI_FORMAT_R8G8B8A8_UNORM;   // jpgファイルのフォーマット
		}
		else if (extension == "bmp") {
			return DXGI_FORMAT_R8G8B8A8_UNORM;   // bmpファイルのフォーマット
		}
		else {
			assert(!"現状対応してない拡張子");
		}
		return DXGI_FORMAT_UNKNOWN;  // コンパイラの警告だるいから適当に返しといてる、どうせassertされる
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
		return; // もともと持っているテクスチャだったらそのまま帰る
	}

	// 持ってなかったら加える
#ifdef _DEBUG
	/*if (Sequence::IsRunTime()) {
		std::cout << "RunTime中のモデルロード : " << ToUTF8(textureName) << "\n";
		assert(!"ランタイム中に新しいテクスチャをロードしないで!!");
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
		return iter->second;  // 持っていたらそれを返す
	}
	AddTexture(textureName);  // 持ってなかったら加えてから返す
	return mTextures[textureName];
}

void TextureManager::RemoveTexture(const std::wstring& textureName) {
	mTextures.erase(textureName);
}
