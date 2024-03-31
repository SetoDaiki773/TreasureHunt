#pragma once
#include <memory>
#include <iostream>

class Component {
public:
	Component();
	virtual ~Component() {};
	virtual void Update() {};  // デフォルトの実装ではなにもしない



};

using ComponentPtr = std::shared_ptr<Component>;