#include "ModelManager.h"
#include "ObjectComponent/Model.h"
#include "Common/FileUtil.h"
#include "Scene/Sequence.h"
#include <iostream>

void ModelManager::Destroy() {
	mModels.clear();
}

//----------------------------------
//  ���f����ǉ�
//----------------------------------
void ModelManager::AddModel(const std::wstring& fileName,const std::wstring& vertexShaderName,const std::wstring& pixelShaderName) {
	auto iter = mModels.find(fileName);
	if (iter != mModels.end()) {
		return;
	}
#ifdef _DEBUG
	/*if (Sequence::IsRunTime()) {
		std::cout << "RunTime���̃��f�����[�h : " << ToUTF8(fileName) << "\n";
		assert(!"�����^�C�����ɐV�������f�������[�h���Ȃ���!!");
	}*/
#endif
	ModelPtrS model = std::make_shared<Model>(fileName.c_str(),vertexShaderName,pixelShaderName);
	mModels.emplace(fileName, model);
}

void ModelManager::AddModel(const std::wstring& modelFile, const std::wstring& shaderName) {
	AddModel(modelFile,shaderName,shaderName);
}
void ModelManager::AddModel(Shape shape, const std::wstring& textureFile, const std::wstring& vertexShaderName, const std::wstring& pixelShaderName) {
	auto iter = mModels.find(textureFile);
	if (iter != mModels.end()) {
		return;
	}
#ifdef _DEBUG
	/*if (Sequence::IsRunTime()) {
		std::cout << "RunTime���̃��f�����[�h : " << ToUTF8(textureFile) << "\n";
		assert(!"�����^�C�����ɐV�������f�������[�h���Ȃ���!!");
	}*/
#endif
	ModelPtrS model = std::make_shared<Model>(shape,textureFile, vertexShaderName, pixelShaderName);
	mModels.emplace(textureFile, model);
}
void ModelManager::AddModel(Shape shape, const std::wstring& textureFile, const std::wstring& shaderName) {
	AddModel(shape,textureFile,shaderName,shaderName);
}

//-------------------------------
//  ���f�����擾
//-------------------------------
ModelPtrS ModelManager::GetModel(const std::wstring& fileName) {
	auto iter = mModels.find(fileName);
	if (iter != mModels.end()) {
		return iter->second;
	}
	else {
#ifdef _DEBUG
		std::cout << ToUTF8(fileName) << "���ǉ�����Ă��Ȃ��BAddModel���Ă���o������\n";
		assert(!"ModelManager::GetModel()");
#endif
		return nullptr;
	}
}