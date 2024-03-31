#include "PlayerInputComponent.h"
#include "GameObject/GameObject.h"
#include "InputComponent.h"
#include "RigidBody.h"
#include "Camera/Camera.h"
#include "Common/MyMath.h"
#include "Common/QuaternionOperation.h"
#include "Common/VectorOperation.h"
#include "AudioComponent.h"
#include "WorldTransform.h"
#include "Input/GamepadButtonCommand.h"
#include "Input/GamepadStickCommand.h"
#include "Input/KeyBoardCommand.h"
#include "Input/Input.h"
#include "Camera/TPSCamera.h"
#include "Common/AudioSystem.h"
#include <iostream>

using namespace DirectX;

constexpr float moveSpeed = 0.1f;
constexpr float retMoveScaler = 2.25f;  // 壁にぶつかったときに戻る量

namespace {
	// 移動方向をもとにgameObjectを前方に向かせる
	void RotateToFront(const XMFLOAT3& moveVec,GameObject& gameObject) {
		float rotateAngle = atan2(-moveVec.x, -moveVec.z); // y軸だけの回転なのでxzベクトルのなす角でok
		XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(0.0f, rotateAngle, 0.0f);
		gameObject.SetQuaternion(quaternion);
	}

	// 歩行音を再生
	void PlayStepAudio(AudioComponent* audio, DynamicRigidBody* rigidBody) {
		if (audio == nullptr) return;
		if (!rigidBody->IsLanding()) return;
		if (audio->IsPlaying("event:/FootStepPerson")) return;
		audio->PlaySoundEvent("event:/FootStepPerson");
	}
}

PlayerInputComponent::PlayerInputComponent(
	GameObject& player, Camera* camera, 
	AudioComponent* audio,
	DynamicRigidBody* rigidBody,
	float jumpPower,bool isActive
	)
	:mOwner(player)
	,mIsActive(isActive)
	,mInputComponent(std::make_shared<InputComponent>(player))
	,mAlwaysValidInputComponent(std::make_shared<InputComponent>(player))
{
	
	// 右移動関数
	auto moveRight = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);
			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMVECTOR cameraToLookPosVec = Vector::ConvertFloat3ToVector(cameraToLookPos);
			XMVECTOR rot = Quaternion::RotateEuler(0.0f, 90.0f, 0.0f);  // y軸周りに90度回転させるクォータニオンで
			// カメラからカメラの注視位置のベクトルを回転させれば右ベクトル。XMFLOAT3に変換する
			XMFLOAT3 moveVec = Vector::ConvertVectorToFloat3(XMVector3Rotate(cameraToLookPosVec, rot)); 
			moveVec = Vector::GetNormalize(moveVec);  // 正規化と
			moveVec = moveSpeed * moveVec;            // moveSpeedを使うことで、コントローラ毎の移動量を同じにする
			player.AddPos(moveVec);
			RotateToFront(moveVec,player);            // 前を向く

		};
	// 左移動関数
	auto moveLeft = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);
			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMVECTOR cameraToLookPosVec = Vector::ConvertFloat3ToVector(cameraToLookPos);
			XMVECTOR rot = Quaternion::RotateEuler(0.0f,90.0f,0.0f);
			XMFLOAT3 moveVec = Vector::ConvertVectorToFloat3(XMVector3Rotate(cameraToLookPosVec, rot));
			moveVec = Vector::GetNormalize(moveVec);
			moveVec = -moveSpeed * moveVec;
			player.AddPos(XMFLOAT3(moveVec));
			RotateToFront(moveVec,player);

		};
	// 前方移動関数
	auto moveFront = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);

			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMFLOAT3 moveVec = cameraToLookPos;                  // カメラからカメラの注視位置方向が前方移動
			moveVec = moveSpeed * Vector::GetNormalize(moveVec); // 正規化することで、コントローラーによる移動量変化を無くす。
			player.AddPos(XMFLOAT3(moveVec));
			RotateToFront(moveVec,player);
		};
	// 後方移動関数
	auto moveBack = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);
			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMFLOAT3 moveVec = cameraToLookPos;
			moveVec = -moveSpeed * Vector::GetNormalize(moveVec);  // カメラからカメラの注視位置ベクトルを-1倍すれば後ろ移動
			player.AddPos(moveVec);
			RotateToFront(moveVec,player);
		};
	// ゲームパッド用の移動関数
	auto move = [&player,camera,audio,rigidBody](DirectX::XMFLOAT2 stickAxis)
		{
			if (camera == nullptr)	return;
			if (Math::NearEqualF(stickAxis.x, 0.0f) && Math::NearEqualF(stickAxis.y, 0.0f)) return;
			PlayStepAudio(audio,rigidBody);
			// TPSカメラなのでカメラの向いてる方向に応じて移動方向を変えなてはいけない
			// 例えばカメラが右を向いてるときに、スティックを右に倒すと、-z方向に移動する
			// [解法] 
			// 1.スティックの倒した方向と+z方向とのなす角を内積から得る
			// 2.このなす角を使ってカメラからプレイヤー方向のベクトルをy軸周りに回転させる
			// 3.上記で得たベクトルこそが移動ベクトル


			XMFLOAT3 stickAxisFloat3(stickAxis.x, 0.0f, stickAxis.y);
			XMFLOAT3 zAxis(0.0f, 0.0f, 1.0f);
			XMFLOAT3 stickAxisNormal = Vector::GetNormalize(stickAxisFloat3);

			float dot = Vector::GetDot(zAxis, stickAxisNormal); // zベクトルとstickの倒した方向のベクトルの内積を得る
			float angle = acos(dot);                            // 倒したスティックの角度(前に倒してる時を0度として)
			
			// 内積は0～180度でしか得られないので-x方向にスティックを倒した場合は角度を-1倍する
			if (stickAxis.x < 0.0f) { 
				angle *= -1.0f;
			}
			XMVECTOR quaternion2 = XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f); // 得られた角度からクォータニオンを取得
			XMFLOAT3 cameraPos = camera->GetPos();
			XMFLOAT3 playerPos = player.GetPos();
			XMVECTOR cameraToPlayer = XMVectorSet(playerPos.x - cameraPos.x, 0.0f, playerPos.z - cameraPos.z, 1.0f);
			// カメラからプレイヤーのベクトルをクォータニオンで回転させる,これこそが移動の方向ベクトル
			XMVECTOR vector = XMVector3Rotate(cameraToPlayer, quaternion2);  
			XMFLOAT3 moveVec = XMFLOAT3(vector.m128_f32[0], vector.m128_f32[1], vector.m128_f32[2]);
			moveVec = moveSpeed * Vector::GetNormalize(moveVec);  // 正規化とかして移動量決定
			
			player.AddPos(moveVec);

			// 移動ベクトルから前方ベクトルを算出して回転させる
			RotateToFront(moveVec,player);
		};
	// ジャンプ関数
	auto Jump = [jumpPower,audio,rigidBody]() 
		{
			if (rigidBody == nullptr)    return;
			if (!rigidBody->IsLanding()) return;
			audio->PlaySoundEvent("event:/se_jump");
			rigidBody->AddForce(DirectX::XMFLOAT3(0.0f,jumpPower,0.0f));
		};
	// 操作権変更関数
	auto changeActive = [this,camera,&player]()
		{
			if (camera == nullptr) return;
			
			// 操作権があるなら
			if(mIsActive == true){
				
				this->mIsActive = false;  // 操作権を消す
				return;
			}
			// 操作権が無いなら
			else {
				// カメラの注意位置を変えて
				TPSCamera* tpsCamera = new TPSCamera(&player);
				camera->ChangeMovement(tpsCamera);
				mIsActive = true; // 操作権を渡す
				return;
			}
		};

	// ゲームパッドのスティックコマンド
	StickType stickType = ELStick;
	GamepadStickCommand moveCommand(move, stickType);
	mInputComponent->AddCommand(std::move(moveCommand)); // 左スティックで移動する

	// 右移動コマンドを追加
	KeyBoardInput moveRightInput{ 'D',InputState::EHold };
	KeyBoardCommand moveRightCommand(moveRight, moveRightInput);
	mInputComponent->AddCommand(std::move(moveRightCommand));  // DをHoldするとmoveRight関数を呼び出すコマンドを追加

	// 左移動コマンドを追加
	KeyBoardInput moveLeftInput{ 'A',InputState::EHold };
	KeyBoardCommand moveLeftCommand(moveLeft, moveLeftInput);
	mInputComponent->AddCommand(std::move(moveLeftCommand));  // AをHoldするとmoveLeft関数を呼び出すコマンドを追加

	// 前方移動コマンドを追加
	KeyBoardInput moveFrontInput{ 'W',InputState::EHold };
	KeyBoardCommand moveFrontCommand(moveFront, moveFrontInput);
	mInputComponent->AddCommand(std::move(moveFrontCommand)); // WをHoldするとmoveFront関数を呼び出すコマンドを追加

	// 後方移動コマンドを追加
	KeyBoardInput moveBackInput{ 'S',InputState::EHold };
	KeyBoardCommand moveBackCommand(moveBack, moveBackInput);
	mInputComponent->AddCommand(std::move(moveBackCommand)); // SをHoldするとmoveBack関数を呼び出すコマンドを追加

	// ジャンプコマンドを追加
	KeyBoardInput jumpKeyInput{ VK_RBUTTON,InputState::EPush };
	KeyBoardCommand jumpKeyCommand(Jump,jumpKeyInput);
	GamepadButtonInput jumpGamepadInput{ XINPUT_GAMEPAD_B,InputState::EPush };
	GamepadButtonCommand jumpButtonCommand(Jump,jumpGamepadInput);
	mInputComponent->AddCommand(std::move(jumpKeyCommand));     // マウス右クリックするとjump関数を呼び出すコマンドを追加
	mInputComponent->AddCommand(std::move(jumpButtonCommand));  // Bボタンを押すとjump関数を呼び出すコマンドを追加

	KeyBoardInput changeActiveInput{ VK_LBUTTON,InputState::EPush };
	KeyBoardCommand changeActiveCommand(changeActive,changeActiveInput);
	GamepadButtonInput changeActiveButton{ XINPUT_GAMEPAD_Y,InputState::EPush };
	GamepadButtonCommand changeActiveButtonCommand{ changeActive,changeActiveButton };
	mAlwaysValidInputComponent->AddCommand(std::move(changeActiveCommand));        // 左クリックするとchangeActive関数を呼び出すコマンドを追加
	mAlwaysValidInputComponent->AddCommand(std::move(changeActiveButtonCommand));  // Yボタンを押すとchangeActive関数を呼び出すコマンドを追加
}

void PlayerInputComponent::Update() {
	mAlwaysValidInputComponent->Update();  // 常に有効な入力要素の更新
	if (mIsActive == true) {
		mInputComponent->Update();         // activeの時だけ呼び出す入力要素
	}
}

bool PlayerInputComponent::IsActive()const {
	return mIsActive;
}