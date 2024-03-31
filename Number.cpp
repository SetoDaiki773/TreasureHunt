#include "Number.h"
#include "Common/FileUtil.h"
#include "Common/MyMath.h"
#include "Camera/Camera.h"
#include "ObjectComponent/Model.h"
#include "ObjectComponent/WorldTransform.h"
#include <iostream>

std::vector<Model> NumberUI::mNumberUIs;
static bool isFirst = true;

NumberUI::NumberUI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale) 
	:mPos(pos)
	,mScale(scale)
	,mNum(0)
{
	// 初めてのインスタンスなら数字テクスチャをつくる
	if (isFirst) {
		mNumberUIs.reserve(10);  // 0～9の10個の数字を作るので10個分予約
		for (int i = 0; i < 10; i++) {
			std::string numberString = std::to_string(i);
			std::string textureName =  "Asset/Number/" + numberString + ".png";
			std::wstring textureNameWide = ToWideString(textureName);
			Model numberUI(Shape::Square,textureNameWide,L"Shader/ForUI.hlsl");
			mNumberUIs.push_back(std::move(numberUI));
		}
		isFirst = false;
	}
}

NumberUI::NumberUI(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale,int num)
	:mPos(pos)
	,mScale(scale)
	,mNum(num)
{
	if (isFirst) {
		mNumberUIs.reserve(10);
		for (int i = 0; i < 10; i++) {
			std::string numberString = std::to_string(i);
			std::string textureName = "Asset/Number/" + numberString + ".png";
			std::wstring textureNameWide = ToWideString(textureName);
			Model numberUI(Shape::Square, textureNameWide, L"Shader/ForUI.hlsl");
			mNumberUIs.push_back(std::move(numberUI));
		}
		isFirst = false;
	}
}

//----------------------------
//  数字描画
//----------------------------
void NumberUI::Render(const Camera& camera, int num) {
	WorldTransform world(mPos,mScale);
	int numDigits = Math::NumDigits(num);

	// digit桁目の数値を描画
	for (int digit = 1; digit <= numDigits; digit++) {
		int renderNumber = Math::GetNumOfDigit(num,digit);  // numのdigit桁目の数値を取得する
		mNumberUIs[renderNumber].Render(world.GetWorldMat(), camera);
		world.AddPos(DirectX::XMFLOAT3(-mScale.x,0.0f,0.0f)); // 描画位置を1桁分ずらす。
		world.UpdateWorldMat();
	}
}

//-------------------------------
//  メンバ変数の数値を描画する
//-------------------------------
void NumberUI::Render(const Camera& camera) {
	this->Render(camera,mNum);
}