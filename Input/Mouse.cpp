#include "Input/MouseMove.h"
#include "Main.h"
#include <winuser.h>
#include <windef.h>

POINT Mouse::mCurrentCursorPos(1280, 720);
POINT Mouse::mPrevCursorPos(1280, 720);

void Mouse::Update() {
	mPrevCursorPos = mCurrentCursorPos;  // �O�t���[���̃J�[�\���ʒu���X�V��
	GetCursorPos(&mCurrentCursorPos);    // ���݃t���[���̃J�[�\���ʒu���X�V����
	// �J�[�\�����㉺���E�̒[�ɗ�����A�J�[�\���̈ʒu�𒆉���
	const bool cursorInXEdge = (mCurrentCursorPos.x >= WindowWidth || mCurrentCursorPos.x <= 100);
	const bool cursorInYEdge = (mCurrentCursorPos.y >= WindowHeight || mCurrentCursorPos.y <= 100);
	if (cursorInXEdge || cursorInYEdge) {  // �J�[�\���̈ʒu���㉺���E�[�Ȃ�
		ResetMousePos();                   // �J�[�\���𒆉���
		GetCursorPos(&mCurrentCursorPos);  // �J�[�\���𒆉��֖߂������A���Έړ���0�ɂȂ�悤�ɂ���
		GetCursorPos(&mPrevCursorPos);     // ����
	}

}

POINT Mouse::GetRelativeMovement() {
	static bool first = true;
	if (first) {
		Update();  // �}�E�X�̈ʒu����x���X�V����Ă��Ȃ�������PrevMousePos�ɐ������l�������Ă��Ȃ��̂ŁA�X�V���Ă���
		first = false;
	}
	POINT retPoint = POINT(mCurrentCursorPos.x - mPrevCursorPos.x, mCurrentCursorPos.y - mPrevCursorPos.y);
	return retPoint;
}

void Mouse::ResetMousePos() {
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 32768, 32768, 0, NULL);
}
