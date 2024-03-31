#include "HeapOffset.h"
#include <iostream>
#include <algorithm>

unsigned HeapOffset::mNextResourceHeapOffset = 1;
std::vector<unsigned> HeapOffset::mReleasedOffset;

HeapOffset::HeapOffset()
	:mHeapOffset(-1)
	
{

	// 解放された要素がないなら
	if (mReleasedOffset.empty()) {
		// オフセットを順番に使う
		mHeapOffset = mNextResourceHeapOffset;
		mNextResourceHeapOffset += 1;
	}
	// あるなら
	else {
		// 解放されたオフセットを割り当てる
		mHeapOffset = mReleasedOffset.back();
		mReleasedOffset.pop_back();
	}
}

HeapOffset::~HeapOffset() {
	
}

//-----------------------------------------
//  使用中のヒープオフセットを解放する
//-----------------------------------------
void HeapOffset::Release() {
	mReleasedOffset.push_back(mHeapOffset);
}


//---------------------------------------------
// 割り当てられたヒープオフセットを取得する
//---------------------------------------------
unsigned HeapOffset::GetHeapOffset() {
	return mHeapOffset;
}

void HeapOffset::PrintReleasedOffset() {
	for (auto r : mReleasedOffset) {
		std::cout << r <<"\n";
	}
}

void HeapOffset::PrintNextHeapOffset() {
	std::cout << mNextResourceHeapOffset <<"\n";
}

