#pragma once
#include <memory>
#include <iostream>

class Component {
public:
	Component();
	virtual ~Component() {};
	virtual void Update() {};  // �f�t�H���g�̎����ł͂Ȃɂ����Ȃ�



};

using ComponentPtr = std::shared_ptr<Component>;