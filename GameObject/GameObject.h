#pragma once
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <DirectXMath.h>
#include <typeIndex>


// 前方宣言
namespace Object {
	enum Type;
};


//=====================================================
/// [機能] オブジェクトのクラスです。
/// 3Dオブジェクトをつくるときはこのクラスを使用します
//=====================================================
class GameObject
{
public:
	enum class State {
		EActive,
		EDead,
		EStop,
		EUnDraw,
		EStopUndraw,
		EOther
	};

	
	//---------------------------------------------------------------------------------------------------
	/// [機能] コンストラクタ。引数を用いてobjectの初期化をします。位置、回転、拡大率を決定できます
	/// [引数１] オブジェクトの見た目(頂点やルートシグネチャなど)のソースです
	/// [引数２] ワールド座標
	//----------------------------------------------------------------------------------------------------
	GameObject(
		std::shared_ptr<class Model> model,
		class WorldTransform&& world,
		int expectedNumComponents = 5
	);

	void Update();

	//---------------------------------------------------
	///[機能] オブジェクトを描画します
	///[引数] カメラ
	//---------------------------------------------------
	void Render(const class Camera& camera); 


	//---------------------------------------------------
	/// [機能] 影をテクスチャにマップする
	/// [引数] カメラ
	//---------------------------------------------------
	void ShadowMap(const class Camera& camera);

	//---------------------------------------------------
	/// [機能] イベントを送信する
	/// [引数] イベント
	//---------------------------------------------------
	void ReceiveEvent(const class Event& event);


	//---------------------------------------------------
	///[機能] オブジェクトを消してしてよいのか取得
	///[戻り値] オブジェクトを消してよいのか
	//---------------------------------------------------
	bool NeedErase()const;

	
	//--------------------------------------------------
	/// ワールドトランスフォームのゲッターセッター
	//--------------------------------------------------
	void SetWorld(const class WorldTransform& world);
	const class WorldTransform& GetWorld()const;
	void SetPos(const DirectX::XMFLOAT3& pos);
	const DirectX::XMFLOAT3& GetPos()const;
	void AddPos(const DirectX::XMFLOAT3& pos);
	void SetQuaternion(const DirectX::XMVECTOR& quaternion);
	DirectX::XMVECTOR GetQuaternion()const;
	void SetScale(const DirectX::XMFLOAT3& scale);
	DirectX::XMFLOAT3 GetScale()const;
	void SetState(State state);
	State GetState();

	DirectX::XMFLOAT3 GetPrevFramePos();

	template <class CompType>
	void AddSharedComponent(std::shared_ptr<CompType> component) {
		mComponents.emplace(typeid(CompType), component); // コンポーネント追加
	}

	template <class CompType>
	void AddRValueComponent(CompType&& component) {
		mComponents.emplace(typeid(CompType), std::make_shared<CompType>(std::move(component))); // コンポーネント追加
	}
	template<class CompType>
	void RemoveComponent() {
		mComponents.erase(typeid(CompType)); // 見つかったら、同じ型のコンポーネントを消す
	}
private:
	
	std::unordered_map<std::type_index,std::shared_ptr<class Component>> mComponents;
	State mState;                                                  // オブジェクトの状態
	std::shared_ptr<class Model> mResource;                        // 見た目
	std::unique_ptr<class WorldTransform> mWorld;                  // ワールドトランスフォーム
	DirectX::XMFLOAT3 mPrevFramePos;
};

using GameObjectPtrS = std::shared_ptr<GameObject>;


