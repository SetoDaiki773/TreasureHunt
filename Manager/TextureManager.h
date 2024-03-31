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
	std::wstring name;   // テクスチャーファイル名
	DXGI_FORMAT format;  // テクスチャフォーマット
	ComPtr<ID3D12Resource> resource;  // テクスチャリソース
	std::unique_ptr<class HeapOffset> heapOffset; // ヒープオフセット
};

using TexturePtr = std::shared_ptr<Texture>;
using Textures = std::unordered_map<std::wstring, TexturePtr>;


//========================================================================================
/// [機能] 複数のテクスチャをまとめて管理。使いまわすテクスチャがあるなら、こいつを使う
//========================================================================================
class TextureManager {
public:
	//---------------------------------------------
	/// [機能] 共通部分の初期化。一度だけ呼び出す
	//---------------------------------------------
	static void Init(HWND* hWnd);
	TextureManager();

	//-----------------------------------------------------
	/// [機能] 引数名と同じファイルパスのテクスチャを追加
	//-----------------------------------------------------
	void AddTexture(const std::wstring& textureName);

	//---------------------------------------------------
	/// [機能] 引数と同じファイルパスのテクスチャを取得
	//---------------------------------------------------
	TexturePtr GetTexture(const std::wstring& textureName);

	//---------------------------------------------------
	/// [機能] 引数と同じファイルパスのテクスチャを削除
	//---------------------------------------------------
	void RemoveTexture(const std::wstring& textureName);

private:
	Textures mTextures;
	static std::shared_ptr<class Renderer> mRenderer;
};
