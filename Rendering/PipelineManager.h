#pragma once
#include <string>
#include <memory>
#include <unordered_map>

using PipelinePtr = std::shared_ptr<class PipelineState>;

//=====================================================
/// [�@�\] �p�C�v���C���X�e�[�g���܂Ƃ߂ĊǗ�����B
//=====================================================
class PipelineManager {
public:

	//-----------------------------------------------------------
	/// [�@�\] �������̃V�F�[�_������p�C�v���C���𐶐����Ēǉ�
	//-----------------------------------------------------------
	void AddPipelineState(const std::wstring& vertexShaderName, const std::wstring& pixelShaderName);

	//---------------------------------------------------------
	// [�@�\] ���_�V�F�[�_�����L�[�Ƃ��ăp�C�v���C�����擾
	//---------------------------------------------------------
	PipelinePtr GetPipelineState(const std::wstring& vertexShaderName);


private:
	std::unordered_map<std::wstring, std::shared_ptr<class PipelineState>> mPipelineStates;
};
