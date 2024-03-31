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
constexpr float retMoveScaler = 2.25f;  // �ǂɂԂ������Ƃ��ɖ߂��

namespace {
	// �ړ����������Ƃ�gameObject��O���Ɍ�������
	void RotateToFront(const XMFLOAT3& moveVec,GameObject& gameObject) {
		float rotateAngle = atan2(-moveVec.x, -moveVec.z); // y�������̉�]�Ȃ̂�xz�x�N�g���̂Ȃ��p��ok
		XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(0.0f, rotateAngle, 0.0f);
		gameObject.SetQuaternion(quaternion);
	}

	// ���s�����Đ�
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
	
	// �E�ړ��֐�
	auto moveRight = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);
			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMVECTOR cameraToLookPosVec = Vector::ConvertFloat3ToVector(cameraToLookPos);
			XMVECTOR rot = Quaternion::RotateEuler(0.0f, 90.0f, 0.0f);  // y�������90�x��]������N�H�[�^�j�I����
			// �J��������J�����̒����ʒu�̃x�N�g������]������ΉE�x�N�g���BXMFLOAT3�ɕϊ�����
			XMFLOAT3 moveVec = Vector::ConvertVectorToFloat3(XMVector3Rotate(cameraToLookPosVec, rot)); 
			moveVec = Vector::GetNormalize(moveVec);  // ���K����
			moveVec = moveSpeed * moveVec;            // moveSpeed���g�����ƂŁA�R���g���[�����̈ړ��ʂ𓯂��ɂ���
			player.AddPos(moveVec);
			RotateToFront(moveVec,player);            // �O������

		};
	// ���ړ��֐�
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
	// �O���ړ��֐�
	auto moveFront = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);

			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMFLOAT3 moveVec = cameraToLookPos;                  // �J��������J�����̒����ʒu�������O���ړ�
			moveVec = moveSpeed * Vector::GetNormalize(moveVec); // ���K�����邱�ƂŁA�R���g���[���[�ɂ��ړ��ʕω��𖳂����B
			player.AddPos(XMFLOAT3(moveVec));
			RotateToFront(moveVec,player);
		};
	// ����ړ��֐�
	auto moveBack = [&player,camera,audio,rigidBody]()
		{
			PlayStepAudio(audio,rigidBody);
			XMFLOAT3 cameraToLookPos = camera->GetLookPos() - camera->GetPos();
			cameraToLookPos.y = 0.0f;
			XMFLOAT3 moveVec = cameraToLookPos;
			moveVec = -moveSpeed * Vector::GetNormalize(moveVec);  // �J��������J�����̒����ʒu�x�N�g����-1�{����Ό��ړ�
			player.AddPos(moveVec);
			RotateToFront(moveVec,player);
		};
	// �Q�[���p�b�h�p�̈ړ��֐�
	auto move = [&player,camera,audio,rigidBody](DirectX::XMFLOAT2 stickAxis)
		{
			if (camera == nullptr)	return;
			if (Math::NearEqualF(stickAxis.x, 0.0f) && Math::NearEqualF(stickAxis.y, 0.0f)) return;
			PlayStepAudio(audio,rigidBody);
			// TPS�J�����Ȃ̂ŃJ�����̌����Ă�����ɉ����Ĉړ�������ς��ȂĂ͂����Ȃ�
			// �Ⴆ�΃J�������E�������Ă�Ƃ��ɁA�X�e�B�b�N���E�ɓ|���ƁA-z�����Ɉړ�����
			// [��@] 
			// 1.�X�e�B�b�N�̓|����������+z�����Ƃ̂Ȃ��p����ς��瓾��
			// 2.���̂Ȃ��p���g���ăJ��������v���C���[�����̃x�N�g����y������ɉ�]������
			// 3.��L�œ����x�N�g���������ړ��x�N�g��


			XMFLOAT3 stickAxisFloat3(stickAxis.x, 0.0f, stickAxis.y);
			XMFLOAT3 zAxis(0.0f, 0.0f, 1.0f);
			XMFLOAT3 stickAxisNormal = Vector::GetNormalize(stickAxisFloat3);

			float dot = Vector::GetDot(zAxis, stickAxisNormal); // z�x�N�g����stick�̓|���������̃x�N�g���̓��ς𓾂�
			float angle = acos(dot);                            // �|�����X�e�B�b�N�̊p�x(�O�ɓ|���Ă鎞��0�x�Ƃ���)
			
			// ���ς�0�`180�x�ł��������Ȃ��̂�-x�����ɃX�e�B�b�N��|�����ꍇ�͊p�x��-1�{����
			if (stickAxis.x < 0.0f) { 
				angle *= -1.0f;
			}
			XMVECTOR quaternion2 = XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f); // ����ꂽ�p�x����N�H�[�^�j�I�����擾
			XMFLOAT3 cameraPos = camera->GetPos();
			XMFLOAT3 playerPos = player.GetPos();
			XMVECTOR cameraToPlayer = XMVectorSet(playerPos.x - cameraPos.x, 0.0f, playerPos.z - cameraPos.z, 1.0f);
			// �J��������v���C���[�̃x�N�g�����N�H�[�^�j�I���ŉ�]������,���ꂱ�����ړ��̕����x�N�g��
			XMVECTOR vector = XMVector3Rotate(cameraToPlayer, quaternion2);  
			XMFLOAT3 moveVec = XMFLOAT3(vector.m128_f32[0], vector.m128_f32[1], vector.m128_f32[2]);
			moveVec = moveSpeed * Vector::GetNormalize(moveVec);  // ���K���Ƃ����Ĉړ��ʌ���
			
			player.AddPos(moveVec);

			// �ړ��x�N�g������O���x�N�g�����Z�o���ĉ�]������
			RotateToFront(moveVec,player);
		};
	// �W�����v�֐�
	auto Jump = [jumpPower,audio,rigidBody]() 
		{
			if (rigidBody == nullptr)    return;
			if (!rigidBody->IsLanding()) return;
			audio->PlaySoundEvent("event:/se_jump");
			rigidBody->AddForce(DirectX::XMFLOAT3(0.0f,jumpPower,0.0f));
		};
	// ���쌠�ύX�֐�
	auto changeActive = [this,camera,&player]()
		{
			if (camera == nullptr) return;
			
			// ���쌠������Ȃ�
			if(mIsActive == true){
				
				this->mIsActive = false;  // ���쌠������
				return;
			}
			// ���쌠�������Ȃ�
			else {
				// �J�����̒��ӈʒu��ς���
				TPSCamera* tpsCamera = new TPSCamera(&player);
				camera->ChangeMovement(tpsCamera);
				mIsActive = true; // ���쌠��n��
				return;
			}
		};

	// �Q�[���p�b�h�̃X�e�B�b�N�R�}���h
	StickType stickType = ELStick;
	GamepadStickCommand moveCommand(move, stickType);
	mInputComponent->AddCommand(std::move(moveCommand)); // ���X�e�B�b�N�ňړ�����

	// �E�ړ��R�}���h��ǉ�
	KeyBoardInput moveRightInput{ 'D',InputState::EHold };
	KeyBoardCommand moveRightCommand(moveRight, moveRightInput);
	mInputComponent->AddCommand(std::move(moveRightCommand));  // D��Hold�����moveRight�֐����Ăяo���R�}���h��ǉ�

	// ���ړ��R�}���h��ǉ�
	KeyBoardInput moveLeftInput{ 'A',InputState::EHold };
	KeyBoardCommand moveLeftCommand(moveLeft, moveLeftInput);
	mInputComponent->AddCommand(std::move(moveLeftCommand));  // A��Hold�����moveLeft�֐����Ăяo���R�}���h��ǉ�

	// �O���ړ��R�}���h��ǉ�
	KeyBoardInput moveFrontInput{ 'W',InputState::EHold };
	KeyBoardCommand moveFrontCommand(moveFront, moveFrontInput);
	mInputComponent->AddCommand(std::move(moveFrontCommand)); // W��Hold�����moveFront�֐����Ăяo���R�}���h��ǉ�

	// ����ړ��R�}���h��ǉ�
	KeyBoardInput moveBackInput{ 'S',InputState::EHold };
	KeyBoardCommand moveBackCommand(moveBack, moveBackInput);
	mInputComponent->AddCommand(std::move(moveBackCommand)); // S��Hold�����moveBack�֐����Ăяo���R�}���h��ǉ�

	// �W�����v�R�}���h��ǉ�
	KeyBoardInput jumpKeyInput{ VK_RBUTTON,InputState::EPush };
	KeyBoardCommand jumpKeyCommand(Jump,jumpKeyInput);
	GamepadButtonInput jumpGamepadInput{ XINPUT_GAMEPAD_B,InputState::EPush };
	GamepadButtonCommand jumpButtonCommand(Jump,jumpGamepadInput);
	mInputComponent->AddCommand(std::move(jumpKeyCommand));     // �}�E�X�E�N���b�N�����jump�֐����Ăяo���R�}���h��ǉ�
	mInputComponent->AddCommand(std::move(jumpButtonCommand));  // B�{�^����������jump�֐����Ăяo���R�}���h��ǉ�

	KeyBoardInput changeActiveInput{ VK_LBUTTON,InputState::EPush };
	KeyBoardCommand changeActiveCommand(changeActive,changeActiveInput);
	GamepadButtonInput changeActiveButton{ XINPUT_GAMEPAD_Y,InputState::EPush };
	GamepadButtonCommand changeActiveButtonCommand{ changeActive,changeActiveButton };
	mAlwaysValidInputComponent->AddCommand(std::move(changeActiveCommand));        // ���N���b�N�����changeActive�֐����Ăяo���R�}���h��ǉ�
	mAlwaysValidInputComponent->AddCommand(std::move(changeActiveButtonCommand));  // Y�{�^����������changeActive�֐����Ăяo���R�}���h��ǉ�
}

void PlayerInputComponent::Update() {
	mAlwaysValidInputComponent->Update();  // ��ɗL���ȓ��͗v�f�̍X�V
	if (mIsActive == true) {
		mInputComponent->Update();         // active�̎������Ăяo�����͗v�f
	}
}

bool PlayerInputComponent::IsActive()const {
	return mIsActive;
}