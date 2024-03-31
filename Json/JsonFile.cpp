#include "JsonFile.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <memory>

JsonFile::JsonFile(const std::string& fileName)
	:mFileName(fileName)
{
	std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		assert(!"Json�t�@�C�����J���Ȃ�����");

	}
	std::ifstream::pos_type fileSize = file.tellg(); // �t�@�C���̃T�C�Y�����߂āA
	file.seekg(0, std::ios::beg);                    // �ŏ��ɖ߂��A
	std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
	file.read(bytes.data(), static_cast<size_t>(fileSize));  // �t�@�C���̃T�C�Y��char�^�z��ɏ������ށB
	rapidjson::Document documentData;
	mFileData.Parse(bytes.data());  // �������܂ꂽ�f�[�^�����ƂɁAJsonFileData���\�z
	if (!mFileData.IsObject()) {
		assert(!"JsonFile�̓ǂݍ��݂��܂������ĂȂ�����");
	}
}


void JsonFile::SaveFile() {
	// �R�s�y���������A�悭�킩���ĂȂ�
	rapidjson::StringBuffer buffer;

	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	mFileData.Accept(writer);
	const char* output = buffer.GetString();

	std::ofstream outFile(mFileName);
	if (outFile.is_open())
	{
		outFile << output;
	}
}

std::string JsonFile::GetFileName() {
	return mFileName;
}

//-------------------------------------------------------------
// Json�t�@�C���̒��g��ǂݏ������邽�߂̃I�u�W�F�N�g��Ԃ�
//--------------------------------------------------------------
JsonObject JsonFile::GetJsonObject() {
	return JsonObject(mFileData);
}


JsonObject::JsonObject(rapidjson::Value& value)
	:mValue(value)
{

}

//---------------------------------------
//  �L�[�ɑΉ�����int�l���擾����
//---------------------------------------
int JsonObject::GetInt(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int�^����Ȃ�");
	}
	return property.GetInt();
}

//--------------------------------------
//  �L�[�ɑΉ�����int�l�ɃZ�b�g����
//--------------------------------------
void JsonObject::SetInt(const char* keyName, int setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int�^����Ȃ�");
	}
	property.SetInt(setValue);
}

//----------------------------------
// �L�[�ɑΉ�����floa�l���擾����
//----------------------------------
float JsonObject::GetFloat(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float�^����Ȃ�");
	}
	return property.GetFloat();
}

//----------------------------------------
//  �L�[�ɑΉ�����float�l�ɃZ�b�g����
//----------------------------------------
void JsonObject::SetFloat(const char* keyName, float setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float�^����Ȃ�");
	}
	property.SetFloat(setValue);
}

//-------------------------------------
//  �L�[�ɑΉ�����bool�l���擾����
//-------------------------------------
bool JsonObject::GetBool(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool�^����Ȃ�");
	}
	return property.GetBool();
}

//----------------------------------
//  �L�[�ɑΉ�����bool�l�ɃZ�b�g����
//----------------------------------
void JsonObject::SetBool(const char* keyName, bool setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool�^����Ȃ�");
	}
	property.SetBool(setValue);
}

//-----------------------------------
//  �L�[�ɑΉ����镶������擾����
//-----------------------------------
std::string JsonObject::GetString(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String�^����Ȃ�");
	}
	return property.GetString();
}

//--------------------------------------
//  �L�[�ɑΉ����镶����ɃZ�b�g����
//--------------------------------------
void JsonObject::SetString(const char* keyName, const rapidjson::GenericStringRef<char>& setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String�^����Ȃ�");
	}
	property.SetString(setValue);
}

//----------------------------------
// Json�z���Ԃ�...
//----------------------------------
rapidjson::GenericArray<false, rapidjson::Value> JsonObject::GetArray(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsArray()) {
		assert(!"�z�񂶂�Ȃ�");
	}
	return property.GetArray();
}

//----------------------------------
//  JsonObject��Ԃ�
//----------------------------------
JsonObject JsonObject::GetObjectData(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsObject()) {
		assert(!"�I�u�W�F�N�g����Ȃ�");
	}
	return JsonObject(property);
}



rapidjson::GenericValue<rapidjson::UTF8<>>& JsonObject::GetProperty(const char* keyName) {
	auto iter = mValue.FindMember(keyName);
	if (iter == mValue.MemberEnd()) {
		assert(!"�t�@�C���f�[�^����L�[�̖��O��������Ȃ��B���O�̊ԈႢ��JsonFile�̃X�R�[�v�O�̉\��������!!!");
	}
	return iter->value;
}