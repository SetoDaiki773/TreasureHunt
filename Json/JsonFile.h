#pragma once
#include <string>
#include <vector>
#include <memory>
#include "packages/rapidjson/document.h"
#include "packages/rapidjson/stringbuffer.h"
#include "packages/rapidjson/prettywriter.h"

//====================================================================================================
/// [機能] Jsonのファイルデータを管理する。Jsonファイル経由で変更したデータを保存したりもできる
//====================================================================================================
class JsonFile {
public:
	explicit JsonFile(const std::string& fileName);

	//------------------------------------------------------------------
	/// [機能] Jsonファイルの中身を読み書き出来る、オブジェクトを返す
	/// [注意] 読み書き元はJsonFileに包含されているので、
	///        JsonObjectよりもJsonFileのスコープを短くしてはいけない。
	//------------------------------------------------------------------
	class JsonObject GetJsonObject();
	void SaveFile();
	std::string GetFileName();

private:
	rapidjson::Document mFileData;
	std::string mFileName;
};


//--------------------------------------------------------------------
/// [*注意*] JsonFileの寿命が切れたらこのクラスは機能しない!!!欠陥!!!!!!!!!!!!!!!!!!!!!!
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
	void* operator new(std::size_t) = delete;  // 天才の発想。new出来なくすることでJsonFile以下の寿命を保証する
	JsonObject operator=(const JsonObject&) = delete; // こいつも寿命を延ばさないための知恵
	JsonObject(const JsonObject&) = delete; // こいつも寿命を延ばさないための知恵
	

	rapidjson::GenericValue<rapidjson::UTF8<>>& GetProperty(const char* keyName);
	rapidjson::Value& mValue;
};
