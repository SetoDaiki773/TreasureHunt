#pragma once
#include <vector>
#include <cassert>
#include <Windows.h>

//------------------------------------------------------------------------------------
/// �g�p��̒���
/// �q�[�v�I�t�Z�b�g����ɂ�Release()���Ăяo���B���̌Ăяo����Y��Ȃ��悤�ɂ��Ă��炢����
/// ���R. �R�s�[�����݂���ꍇ�A�����I�t�Z�b�g����������Ă��܂�����
//-----------------------------------------------------------------------------------

//=============================================================================================
/// [�@�\] �f�B�X�N���v�^�q�[�v�Ǘ��N���X�B�]���Ă���q�[�v�I�t�Z�b�g��Ԃ��Ă����B
///        �F�X�g���Â炢����A���̂����Ȃ��������B
//=============================================================================================
class HeapOffset {
public:
	HeapOffset();
	~HeapOffset();

	HeapOffset(const HeapOffset&) = default;

	void Release();

	unsigned GetHeapOffset();
	static void PrintReleasedOffset();
	static void PrintNextHeapOffset();


private:
	unsigned mHeapOffset;                      // �g�p���Ă���q�[�v�I�t�Z�b�g
	static unsigned mNextResourceHeapOffset;   // ���̃��\�[�X�̃q�[�v�I�t�Z�b�g
	static std::vector<unsigned> mReleasedOffset;  // ������ꂽ�q�[�v�I�t�Z�b�g
	
};



