#pragma once
#include <memory>

//===============================================
/// [機能] 鍵の数を色ごとにカウントしてくれる。
//===============================================
struct KeyCounter {
	unsigned redKeyCount = 0;
	unsigned blueKeyCount = 0;
};

using KeyCounterPtr = std::shared_ptr<KeyCounter>;
