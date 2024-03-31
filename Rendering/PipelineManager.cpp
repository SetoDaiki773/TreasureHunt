#include "PipelineManager.h"
#include "PipelineState.h"
#include "Common/FileUtil.h"
#include <cassert>
#include <iostream>

//---------------------------------------
//  �p�C�v���C���X�e�[�g��ǉ�
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
//  �p�C�v���C���X�e�[�g���擾
//---------------------------------------
std::shared_ptr<class PipelineState> PipelineManager::GetPipelineState(const std::wstring& vertexShaderName) {
	auto iter = mPipelineStates.find(vertexShaderName); // ���_�V�F�[�_�\�������ƂɃp�C�v���C�����擾
	if (iter == mPipelineStates.end()) {
		std::wstring errorMessage = vertexShaderName + L"��������Ȃ������BAddPipelineState�ŉ����Ă���A���̊֐����Ă�ł�";
		std::cout << ToUTF8(errorMessage) <<"\n";
		assert(!"PipelineManager.cpp");
	}
	return iter->second; // ���������Ԃ�����
}
