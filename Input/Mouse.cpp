#include "Input/MouseMove.h"
#include "Main.h"
#include <winuser.h>
#include <windef.h>

POINT Mouse::mCurrentCursorPos(1280, 720);
POINT Mouse::mPrevCursorPos(1280, 720);

void Mouse::Update() {
	mPrevCursorPos = mCurrentCursorPos;  // 前フレームのカーソル位置を更新し
	GetCursorPos(&mCurrentCursorPos);    // 現在フレームのカーソル位置を更新する
	// カーソルが上下左右の端に来たら、カーソルの位置を中央へ
	const bool cursorInXEdge = (mCurrentCursorPos.x >= WindowWidth || mCurrentCursorPos.x <= 100);
	const bool cursorInYEdge = (mCurrentCursorPos.y >= WindowHeight || mCurrentCursorPos.y <= 100);
	if (cursorInXEdge || cursorInYEdge) {  // カーソルの位置が上下左右端なら
		ResetMousePos();                   // カーソルを中央へ
		GetCursorPos(&mCurrentCursorPos);  // カーソルを中央へ戻したが、相対移動は0になるようにする
		GetCursorPos(&mPrevCursorPos);     // 同上
	}

}

POINT Mouse::GetRelativeMovement() {
	static bool first = true;
	if (first) {
		Update();  // マウスの位置が一度も更新されていなかったらPrevMousePosに正しい値が入っていないので、更新しておく
		first = false;
	}
	POINT retPoint = POINT(mCurrentCursorPos.x - mPrevCursorPos.x, mCurrentCursorPos.y - mPrevCursorPos.y);
	return retPoint;
}

void Mouse::ResetMousePos() {
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 32768, 32768, 0, NULL);
}
