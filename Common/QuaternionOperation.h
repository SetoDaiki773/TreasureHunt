#pragma once
#include <DirectXmath.h>

namespace Quaternion {


	//---------------------------------------------
	/// [�@�\] �O���x�N�g�����N�H�[�^�j�I���Ŏ擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] �O���x�N�g���ł�
	//---------------------------------------------
	DirectX::XMFLOAT3 GetFront(const DirectX::XMVECTOR& quaternion);


	//----------------------------------------
	/// [�@�\] ����x�N�g�����擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ����x�N�g���ł�
	//----------------------------------------
	DirectX::XMFLOAT3 GetBack(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------
	/// [�@�\] �E�x�N�g�����擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] �E�x�N�g���ł�
	//----------------------------------------
	DirectX::XMFLOAT3 GetRight(const DirectX::XMVECTOR& quaternion);

	//----------------------------------------
	/// [�@�\] ���x�N�g�����擾���܂�
	/// [����] �N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ���x�N�g���ł�
	//----------------------------------------
	DirectX::XMFLOAT3 GetLeft(const DirectX::XMVECTOR& quaternion);


	//-----------------------------------------------
	/// [�@�\] �x���@�ŉ�]�l�������擾���܂�
	//-----------------------------------------------
	DirectX::XMVECTOR RotateEuler(float pitch, float yaw, float roll);
	DirectX::XMVECTOR RotateAxis(const DirectX::XMVECTOR& axis, float degreeAngle);


	//-------------------------------------------------------------------
	/// [�@�\] �I�C���[�p�ŉ�]�����A�\�[�X�̃N�H�[�^�j�I���Ə�Z���܂�
	/// [����1,2,3] ���[���A�s�b�`�A���[������̉�]�ʂł�(�x���@) 
	/// [����4] �Z�o���ꂽ�N�H�[�^�j�I���Ə�Z�������N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ��Z���ʂł�
	//--------------------------------------------------------------------
	DirectX::XMVECTOR RotateEulerAndMultiply(float pitch, float yaw, float roll, const DirectX::XMVECTOR& srcQuaternion);

	//-----------------------------------------------------------------
	/// [�@�\] �C�ӎ��ŉ�]�����A�\�[�X�̃N�H�[�^�j�I���Ə�Z���܂�
	/// [����1] ��]���ł�
	/// [����2] ��]��(�x���@)
	/// [����2] �Z�o���ꂽ�N�H�[�^�j�I���Ə�Z�������N�H�[�^�j�I���ł�
	/// [�Ԃ�l] ��Z���ʂł�
	//-----------------------------------------------------------------
	DirectX::XMVECTOR RotateAxisAndMultiply(const DirectX::XMVECTOR& axis, float degreeAngle, const DirectX::XMVECTOR& srcQuaternion);


}  // end Quaternion namespace
