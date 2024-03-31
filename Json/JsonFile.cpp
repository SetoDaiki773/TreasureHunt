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
		assert(!"Jsonファイルが開けなかった");

	}
	std::ifstream::pos_type fileSize = file.tellg(); // ファイルのサイズを求めて、
	file.seekg(0, std::ios::beg);                    // 最初に戻し、
	std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
	file.read(bytes.data(), static_cast<size_t>(fileSize));  // ファイルのサイズ分char型配列に書き込む。
	rapidjson::Document documentData;
	mFileData.Parse(bytes.data());  // 書き込まれたデータをもとに、JsonFileDataを構築
	if (!mFileData.IsObject()) {
		assert(!"JsonFileの読み込みうまくいってないかも");
	}
}


void JsonFile::SaveFile() {
	// コピペしただけ、よくわかってない
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
// Jsonファイルの中身を読み書きするためのオブジェクトを返す
//--------------------------------------------------------------
JsonObject JsonFile::GetJsonObject() {
	return JsonObject(mFileData);
}


JsonObject::JsonObject(rapidjson::Value& value)
	:mValue(value)
{

}

//---------------------------------------
//  キーに対応するint値を取得する
//---------------------------------------
int JsonObject::GetInt(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int型じゃない");
	}
	return property.GetInt();
}

//--------------------------------------
//  キーに対応するint値にセットする
//--------------------------------------
void JsonObject::SetInt(const char* keyName, int setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsInt()) {
		assert(!"int型じゃない");
	}
	property.SetInt(setValue);
}

//----------------------------------
// キーに対応するfloa値を取得する
//----------------------------------
float JsonObject::GetFloat(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float型じゃない");
	}
	return property.GetFloat();
}

//----------------------------------------
//  キーに対応するfloat値にセットする
//----------------------------------------
void JsonObject::SetFloat(const char* keyName, float setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsFloat()) {
		assert(!"float型じゃない");
	}
	property.SetFloat(setValue);
}

//-------------------------------------
//  キーに対応するbool値を取得する
//-------------------------------------
bool JsonObject::GetBool(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool型じゃない");
	}
	return property.GetBool();
}

//----------------------------------
//  キーに対応するbool値にセットする
//----------------------------------
void JsonObject::SetBool(const char* keyName, bool setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsBool()) {
		assert(!"bool型じゃない");
	}
	property.SetBool(setValue);
}

//-----------------------------------
//  キーに対応する文字列を取得する
//-----------------------------------
std::string JsonObject::GetString(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String型じゃない");
	}
	return property.GetString();
}

//--------------------------------------
//  キーに対応する文字列にセットする
//--------------------------------------
void JsonObject::SetString(const char* keyName, const rapidjson::GenericStringRef<char>& setValue) {
	auto& property = GetProperty(keyName);
	if (!property.IsString()) {
		assert(!"String型じゃない");
	}
	property.SetString(setValue);
}

//----------------------------------
// Json配列を返す...
//----------------------------------
rapidjson::GenericArray<false, rapidjson::Value> JsonObject::GetArray(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsArray()) {
		assert(!"配列じゃない");
	}
	return property.GetArray();
}

//----------------------------------
//  JsonObjectを返す
//----------------------------------
JsonObject JsonObject::GetObjectData(const char* keyName) {
	auto& property = GetProperty(keyName);
	if (!property.IsObject()) {
		assert(!"オブジェクトじゃない");
	}
	return JsonObject(property);
}



rapidjson::GenericValue<rapidjson::UTF8<>>& JsonObject::GetProperty(const char* keyName) {
	auto iter = mValue.FindMember(keyName);
	if (iter == mValue.MemberEnd()) {
		assert(!"ファイルデータからキーの名前が見つからない。名前の間違いやJsonFileのスコープ外の可能性がある!!!");
	}
	return iter->value;
}