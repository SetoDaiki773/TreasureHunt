#include "PipelineManager.h"
#include "PipelineState.h"
#include "Common/FileUtil.h"
#include <cassert>
#include <iostream>

//---------------------------------------
//  パイプラインステートを追加
//---------------------------------------
void PipelineManager::AddPipelineState(const std::wstring& vertexShaderName, const std::wstring& pixelShaderName) {

	auto iter = mPipelineStates.find(vertexShaderName);
	if (iter != mPipelineStates.end()) {
		return;
	}
	PipelinePtr pipelineState = std::make_shared<PipelineState>(vertexShaderName,pixelShaderName);
	mPipelineStates[vertexShaderName] = pipelineState;
}

//---------------------------------------
//  パイプラインステートを取得
//---------------------------------------
std::shared_ptr<class PipelineState> PipelineManager::GetPipelineState(const std::wstring& vertexShaderName) {
	auto iter = mPipelineStates.find(vertexShaderName); // 頂点シェーダ―名をもとにパイプラインを取得
	if (iter == mPipelineStates.end()) {
		std::wstring errorMessage = vertexShaderName + L"が見つからなかった。AddPipelineStateで加えてから、この関数を呼んでね";
		std::cout << ToUTF8(errorMessage) <<"\n";
		assert(!"PipelineManager.cpp");
	}
	return iter->second; // 見つけたやつを返すだけ
}
