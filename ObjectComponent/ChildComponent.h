#pragma once
#include "ObjectComponent/Component.h"
#include <memory>

//========================================================================
/// [機能] コンストラクタの第一引数は第二引数を親とする、子供になる。
///        子供は親と同じ分だけ移動する。
//========================================================================
class ChildComponent : public Component {
public:
	            // 子供が親を持っているだけで、オブジェクトが存在する必要はないので、weak_ptrにする
	ChildComponent(class GameObject& owner,std::weak_ptr<class GameObject> parent);

	void Update()override;


private:
	class GameObject& mOwner;
	std::weak_ptr<class GameObject> mParent;

};