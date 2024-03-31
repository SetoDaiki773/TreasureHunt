#pragma once
#include "CameraMovement.h"
#include <memory>

//==================================================================================
/// [�@�\] �h���N�G11�Ƃ��Ɠ����悤�ȃJ�������������s����N���X�BtargetObject�������_
//==================================================================================
class TPSCamera : public CameraMovement{
public:
	explicit TPSCamera(class GameObject* targetObject);
	TPSCamera(class GameObject* targetObject,float pitch,float yaw);
	void Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos)override;

	//------------------------------------------------------
	/// [�⑫] static�Ȃ̂�TPS�J��������]�ʂ��󂯌p������
	//------------------------------------------------------
	static void ResetQuaternion();

private:
	class GameObject* mTargetObject;      // null�̉\��������̂Ń|�C���^�Ŏ󂯎��
	static DirectX::XMVECTOR mQuaternion; // �J�����̉�]�ʂ͑S�ẴC���X�^���X�ŋ��ʉ�����
	static float mRoll;                   // ��]�������p���̂Ń��[�������R�����p��
};