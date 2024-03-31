#pragma once
#include <string>
#include <memory>
#include <unordered_map>

using PipelinePtr = std::shared_ptr<class PipelineState>;

//=====================================================
/// [機能] パイプラインステートをまとめて管理する。
//=====================================================
class PipelineManager {
public:

	//-----------------------------------------------------------
	/// [機能] 引数名のシェーダ名からパイプラインを生成して追加
	//-----------------------------------------------------------
	void AddPipelineState(const std::wstring& vertexShaderName, const std::wstring& pixelShaderName);

	//---------------------------------------------------------
	// [機能] 頂点シェーダ名をキーとしてパイプラインを取得
	//---------------------------------------------------------
	PipelinePtr GetPipelineState(const std::wstring& vertexShaderName);


private:
	std::unordered_map<std::wstring, std::shared_ptr<class PipelineState>> mPipelineStates;
};
