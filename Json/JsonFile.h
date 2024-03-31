#pragma once
#include <string>
#include <vector>
#include <memory>
#include "packages/rapidjson/document.h"
#include "packages/rapidjson/stringbuffer.h"
#include "packages/rapidjson/prettywriter.h"

//====================================================================================================
/// [�@�\] Json�̃t�@�C���f�[�^���Ǘ�����BJson�t�@�C���o�R�ŕύX�����f�[�^��ۑ���������ł���
//====================================================================================================
class JsonFile {
public:
	explicit JsonFile(const std::string& fileName);

	//------------------------------------------------------------------
	/// [�@�\] Json�t�@�C���̒��g��ǂݏ����o����A�I�u�W�F�N�g��Ԃ�
	/// [����] �ǂݏ�������JsonFile�ɕ�܂���Ă���̂ŁA
	///        JsonObject����JsonFile�̃X�R�[�v��Z�����Ă͂����Ȃ��B
	//------------------------------------------------------------------
	class JsonObject GetJsonObject();
	void SaveFile();
	std::string GetFileName();

private:
	rapidjson::Document mFileData;
	std::string mFileName;
};


//--------------------------------------------------------------------
/// [*����*] JsonFile�̎������؂ꂽ�炱�̃N���X�͋@�\���Ȃ�!!!����!!!!!!!!!!!!!!!!!!!!!!
//--------------------------------------------------------------------
class JsonObject {
public:
	JsonObject(rapidjson::Value& value);
	int GetInt(const char* keyName);
	void SetInt(const char* keyName, int setValue);
	float GetFloat(const char* keyName);
	void SetFloat(const char* keyName, float setValue);
	bool GetBool(const char* keyName);
	void SetBool(const char* keyName, bool setValue);
	std::string GetString(const char* keyName);
	void SetString(const char* keyName, const rapidjson::GenericStringRef<char>& setValue);
	JsonObject GetObjectData(const char* keyName);
	rapidjson::GenericArray<false, rapidjson::Value> GetArray(const char* keyName);

private:
	void* operator new(std::size_t) = delete;  // �V�˂̔��z�Bnew�o���Ȃ����邱�Ƃ�JsonFile�ȉ��̎�����ۏ؂���
	JsonObject operator=(const JsonObject&) = delete; // ���������������΂��Ȃ����߂̒m�b
	JsonObject(const JsonObject&) = delete; // ���������������΂��Ȃ����߂̒m�b
	

	rapidjson::GenericValue<rapidjson::UTF8<>>& GetProperty(const char* keyName);
	rapidjson::Value& mValue;
};
