#pragma once
#include <vector>
#include <DirectXMath.h>
#include <memory>

//==========================================================
/// [�@�\] ������`��ł���B
//==========================================================
class NumberUI {
public:
	NumberUI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale);
	NumberUI(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale,int num);
	NumberUI(const NumberUI&) = default;

	//--------------------------------------------------------------------
	/// [�@�\] �������Ŏw�肵���ʒu�ɑ�O�����Ŏw�肵�����l��`�悷��
	//--------------------------------------------------------------------
	void Render(const class Camera& camera,int num);

	//-----------------------------------------------------------
	// [�@�\] �����o�ϐ��̐��l��p���ĕ`�悷��
	//-----------------------------------------------------------
	void Render(const class Camera& camera);

private:
	DirectX::XMFLOAT3 mPos;
	DirectX::XMFLOAT3 mScale;
	int mNum;
	static std::vector<class Model> mNumberUIs;  // �����������ꂽ�e�N�X�`���R 0�`9�܂�
};
