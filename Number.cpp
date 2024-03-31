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
	// ���߂ẴC���X�^���X�Ȃ琔���e�N�X�`��������
	if (isFirst) {
		mNumberUIs.reserve(10);  // 0�`9��10�̐��������̂�10���\��
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
//  �����`��
//----------------------------
void NumberUI::Render(const Camera& camera, int num) {
	WorldTransform world(mPos,mScale);
	int numDigits = Math::NumDigits(num);

	// digit���ڂ̐��l��`��
	for (int digit = 1; digit <= numDigits; digit++) {
		int renderNumber = Math::GetNumOfDigit(num,digit);  // num��digit���ڂ̐��l���擾����
		mNumberUIs[renderNumber].Render(world.GetWorldMat(), camera);
		world.AddPos(DirectX::XMFLOAT3(-mScale.x,0.0f,0.0f)); // �`��ʒu��1�������炷�B
		world.UpdateWorldMat();
	}
}

//-------------------------------
//  �����o�ϐ��̐��l��`�悷��
//-------------------------------
void NumberUI::Render(const Camera& camera) {
	this->Render(camera,mNum);
}