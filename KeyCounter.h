#pragma once
#include <memory>

//===============================================
/// [�@�\] ���̐���F���ƂɃJ�E���g���Ă����B
//===============================================
struct KeyCounter {
	unsigned redKeyCount = 0;
	unsigned blueKeyCount = 0;
};

using KeyCounterPtr = std::shared_ptr<KeyCounter>;
