#pragma once
#include <unordered_map>
#include <string>
#include <memory>

class ModelManager {
public:
	ModelManager() {};
	~ModelManager() {};
	void Destroy();

	//---------------------------------------
	/// [機能] モデルを追加
	//---------------------------------------
	void AddModel(const std::wstring& modelFile,const std::wstring& vertexShaderName,const std::wstring& pixelShaderName);
	void AddModel(const std::wstring& modelFile,const std::wstring& shaderName);
	void AddModel(enum class Shape shape,const std::wstring& textureFile,const std::wstring& vertexShaderName,const std::wstring& pixelShaderName);
	void AddModel(enum class Shape shape,const std::wstring& textureFile,const std::wstring& shaderName);

	//--------------------------------------------------------------------
	/// [機能] テクスチャやモデルのファイルパスをキーとしてモデルを取得
	//--------------------------------------------------------------------
	std::shared_ptr<class Model> GetModel(const std::wstring& filePath);
	//void DeleteModel(const std::wstring& filePath);


	

private:
	std::unordered_map<std::wstring, std::shared_ptr<class Model>> mModels;
};
