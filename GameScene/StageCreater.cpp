#include "Stage.h"
#include "StageCreater.h"
#include "KeyAndLockColor.h"
#include "Common/QuaternionOperation.h"

using namespace DirectX;

void StageCreater::CreateStage(Stage& stage1)const {
	stage1.CreatePerson(XMFLOAT3(0.0f, 3.0f, 4.0f));
	stage1.CreateKaeru(XMFLOAT3(5.0f, 5.0f, -6.5f));
	stage1.CreateBlock(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(15.0f,1.0f,15.0f));
	stage1.CreateSkyMap();
	stage1.CreateStatusUI();
	stage1.CreateTreasureBox(XMFLOAT3(11.0f, 18.0f, -9.0f));
	stage1.CreateStair(7, XMFLOAT3(-1.0f, 2.0f, -9.0f));
}

void Stage2Creater::CreateStage(Stage& stage2)const {
	stage2.CreatePerson(XMFLOAT3(15.5f,3.0f,0.0f));
	stage2.CreateKaeru(XMFLOAT3(15.5f,3.0f,3.0f));
	stage2.CreateBlock(XMFLOAT3(12.5f,0.0f,0.0f), XMFLOAT3(5.0f,2.0f,5.0f));
	stage2.CreateBlock(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(5.0f,2.0f,5.0f));
	stage2.CreateBlock(XMFLOAT3(-10.0f,2.0f,0.0f),XMFLOAT3(5.0f,4.0f,5.0f));
	stage2.CreateTreasureBox(XMFLOAT3(-10.0f,12.0f,0.0f));
	stage2.CreateStatusUI();
	stage2.CreateSkyMap();
}

void Stage3Creater::CreateStage(Stage& stage3)const {
	stage3.CreatePerson(XMFLOAT3(0.0f,3.0f,9.0f));
	stage3.CreateKaeru(XMFLOAT3(-2.0f,3.0f,9.0f));
	stage3.CreateBlock(XMFLOAT3(0.0f,0.0f,4.0f),XMFLOAT3(10.0f,1.0f,10.0f));  // ��
	stage3.CreateStair(3, XMFLOAT3(5.0f, 2.0f, 9.0f)); // �n�܂��č���ɂ���K�i
	stage3.CreateStair(3, XMFLOAT3(5.0f, 2.0f, 11.0f)); // �n�܂��č���ɂ���K�i
	stage3.CreateBlock(XMFLOAT3(0.0f,7.0f,2.0f),XMFLOAT3(10.0f,6.0f,6.0f));   // ���ʂɂ��邭���ł���
	stage3.CreateKey(XMFLOAT3(0.0f, 14.5f, 0.0f), Color::ERed); // �����ł��ǂ̏�ɂ���̃J�M
	// �󔠂��͂��ǂ⃍�b�N
	stage3.CreateBlock(XMFLOAT3(9.0f,14.0f,0.0f));
	stage3.CreateBlock(XMFLOAT3(9.0f,16.0f,0.0f));
	stage3.CreateBlock(XMFLOAT3(11.0f,16.0f,0.0f));
	stage3.CreateBlock(XMFLOAT3(13.0f,18.0f,0.0f));
	stage3.CreateBlock(XMFLOAT3(11.0f,20.0f,0.0f));
	stage3.CreateBlock(XMFLOAT3(11.0f,18.0f,2.0f));
	stage3.CreateBlock(XMFLOAT3(11.0f,18.0f,-2.0f));
	stage3.CreateBlock(XMFLOAT3(9.0f,20.0f,0.0f));
	stage3.CreateBlock(XMFLOAT3(7.0f,16.0f,0.0f));
	stage3.CreateLock(XMFLOAT3(9.0f,18.0f,0.0f),Color::EBlue);

	stage3.CreateTreasureBox(XMFLOAT3(11.0f,18.0f,-0.25f),Quaternion::RotateEuler(0.0f,90.f,0.0f)); // �͂�ꂽ��

	stage3.CreateBlock(XMFLOAT3(-15.5f,-4.0f,-2.0f),XMFLOAT3(5.0f,1.0f,7.0f)); // �n�܂��ĉE��ɂ��镂�V��
	stage3.CreateBlock(XMFLOAT3(-15.5f,-4.0f,10.0f),XMFLOAT3(3.0f,0.5f,3.0f));
	stage3.CreateKey(XMFLOAT3(-16.5f, -2.0f, 0.0f), Color::EBlue); // ���V�ǂ̏�̃J�M

	stage3.CreateLock(XMFLOAT3(-9.0f,2.0f,-5.0f),Color::ERed); // ���V�ǂ��炭���ł��ǂ̌��������ɍs�����Ƃ�W���郍�b�N
	stage3.CreateLock(XMFLOAT3(0.0f,4.0f,-5.0f),Color::EBlue); // �����ł��ǂ̌��������̃��b�N
	stage3.CreateStair(4,XMFLOAT3(0.0f,2.0f,-5.0f));           // �����ł��ǂ̌��������̊K�i
	stage3.CreateBlock(XMFLOAT3(2.0f,6.0f,-5.0f));             // �K�i�r���ɂ���u���b�N
	
	stage3.CreateStatusUI();
	stage3.CreateSkyMap();
}


void Stage4Creater::CreateStage(Stage& stage4)const {
	stage4.CreatePerson(XMFLOAT3(-1.0f,3.0f,0.0f));
	stage4.CreateKaeru(XMFLOAT3(-1.0f,5.0f,0.0f));
	stage4.CreateStatusUI();
	stage4.CreateSkyMap();
	stage4.CreateTreasureBox(XMFLOAT3(-10.0f,15.0f,-23.0f));
	stage4.CreateLock(XMFLOAT3(-10.0f,17.0f,-23.0f),Color::ERed); // ��̐^��̃��b�N
	stage4.CreateBlock(XMFLOAT3(-10.0f,13.0f,-23.0f)); // ����悹��u���b�N
	stage4.CreateBlock(XMFLOAT3(-10.0f,15.0f,-25.0f),XMFLOAT3(1.0f,3.0f,1.0f));// ������u���b�N
	stage4.CreateBlock(XMFLOAT3(-8.0f,15.0f,-23.0f));                          // ������u���b�N
	stage4.CreateBlock(XMFLOAT3(-12.0f,15.0f,-23.0f));                         // ������u���b�N
	stage4.CreateBarrel(XMFLOAT3(3.0f,20.0f,-15.0f));                      // ����ɂ����̏�ɏ�����M
	stage4.CreateBlock(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(8.0f,1.0f,8.0f)); // ��
	stage4.CreateBlock(XMFLOAT3(3.0f, 17.0f, -14.0f), XMFLOAT3(2.0f, 1.0f, 8.0f)); // �M���悹������ɂ��鏰
	stage4.CreateBlock(XMFLOAT3(-10.0f,4.75f,0.0f),XMFLOAT3(1.0f,1.75f,1.0f),L"Asset/Wood.png");     // �x��
	stage4.CreateBlock(XMFLOAT3(-10.0f, 6.5f, 0.0f),XMFLOAT3(3.0f, 0.5f, 3.0f),L"Asset/Dirt.png");// �x���̏�̑�
	stage4.CreateBlock(XMFLOAT3(-11.0f,0.0f,0.0f),XMFLOAT3(3.0f,1.0f,1.0f)); // �����ʒu�ɂ��鏰�Ǝx�����ɂ��鏰���Ȃ���
	stage4.CreateBlock(XMFLOAT3(-17.0f,0.0f,0.0f),XMFLOAT3(3.0f,1.0f,3.0f)); // �x�����ɂ���M�Ƃ��悹����
	stage4.CreateBarrel(XMFLOAT3(-17.0f,5.0f,0.0f));                         // �x�����̏�����M
	stage4.CreateKey(XMFLOAT3(-14.0f,2.0f,0.0f),Color::ERed); // �x�����̃��b�N�̂��΂ɂ��錮
	stage4.CreateLock(XMFLOAT3(-10.0f,8.f,0.0f),Color::ERed); // �x����̃��b�N
	stage4.CreateLock(XMFLOAT3(-10.0f,2.0f,0.0f),Color::ERed); // �x�����̃��b�N
	stage4.CreateBlock(XMFLOAT3(-10.0f, 12.0f, -9.0f), XMFLOAT3(3.0f, 1.0f, 6.0f)); // �x����̏�̕��V��
	stage4.CreateBlock(XMFLOAT3(-5.0f, 17.0f, -19.0f),XMFLOAT3(6.0f,1.0f,3.0f)); // �x����̏�̕��V�䂩��ڂ�鏰
	
}