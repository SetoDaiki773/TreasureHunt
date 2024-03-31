#pragma once
#include <DirectXMath.h>

//----------------------------------------------------------------------------
/// �x�N�g���̌v�Z
//----------------------------------------------------------------------------
const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator+=(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator-=(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
const DirectX::XMFLOAT3 operator*(float scaler, const DirectX::XMFLOAT3& vector);
const DirectX::XMFLOAT3 operator/(float scaler, const DirectX::XMFLOAT3& vector);
const DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& vector,float scaler);


namespace Vector {
	
	//-------------------------------------------
	/// [�@�\] 3�����x�N�g�����R���\�[���ɕ\��
	/// [����] 3�����x�N�g��
	//-------------------------------------------
	void Print(const DirectX::XMFLOAT3& vector);

	//---------------------------------------------
	/// [�@�\] �x�N�g���̒����̎擾
	/// [����] 3�����x�N�g��
	/// [�Ԃ�l] �x�N�g���̒���
	//---------------------------------------------
	float GetLenght(const DirectX::XMFLOAT3& vector);

	//----------------------------------------------------------
	/// [�@�\] �x�N�g���̒����̓����擾�BGetLenght��������
	/// [����] 3�����x�N�g��
	/// [�Ԃ�l] �x�N�g���̒����̓��
	//----------------------------------------------------------
	float GetLenghtSquare(const DirectX::XMFLOAT3& vector);

	//---------------------------------------------
	/// [�@�\] �x�N�g���̐��K��
	/// [����] 3�����x�N�g��
	/// [�Ԃ�l] ���K�����ꂽ�x�N�g��
	//---------------------------------------------
	DirectX::XMFLOAT3 GetNormalize(const DirectX::XMFLOAT3& vector);


	//---------------------------------------------
	/// [�@�\] �x�N�g���̓��ς̎擾
	/// [����1] 3�����x�N�g��1��
	/// [����2] 3�����x�N�g��2��
	/// [�߂�l] �x�N�g���̓���
	//---------------------------------------------
	constexpr float GetDot(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);

	//----------------------------------------------
	/// [�@�\] ���g����ڕW���ւ̊p�x�����߂�
	/// [����1] �����̈ʒu�x�N�g��
	/// [����2] �ڕW���̈ʒu�x�N�g��
	/// [�߂�l] �ڕW���̂���p�x
	//----------------------------------------------
	DirectX::XMFLOAT3 GetAngleToTarget(const DirectX::XMFLOAT3& ownVector, const DirectX::XMFLOAT3& targetVector);


	//---------------------------------------------
	/// [�@�\] XMVECTOR�^��XMFLOAT3�^�ɕϊ����܂�
	/// [����] �ϊ��������x�N�g���ł�
	/// [�Ԃ�l] �ϊ����ꂽXMFLOAT3�ł�
	//---------------------------------------------
	DirectX::XMFLOAT3 ConvertVectorToFloat3(const DirectX::XMVECTOR& vector);
	DirectX::XMVECTOR ConvertFloat3ToVector(const DirectX::XMFLOAT3& float3);

}
