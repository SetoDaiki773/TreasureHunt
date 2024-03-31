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
	stage3.CreateBlock(XMFLOAT3(0.0f,0.0f,4.0f),XMFLOAT3(10.0f,1.0f,10.0f));  // 床
	stage3.CreateStair(3, XMFLOAT3(5.0f, 2.0f, 9.0f)); // 始まって左手にある階段
	stage3.CreateStair(3, XMFLOAT3(5.0f, 2.0f, 11.0f)); // 始まって左手にある階段
	stage3.CreateBlock(XMFLOAT3(0.0f,7.0f,2.0f),XMFLOAT3(10.0f,6.0f,6.0f));   // 正面にあるくそでか壁
	stage3.CreateKey(XMFLOAT3(0.0f, 14.5f, 0.0f), Color::ERed); // くそでか壁の上にあるのカギ
	// 宝箱を囲う壁やロック
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

	stage3.CreateTreasureBox(XMFLOAT3(11.0f,18.0f,-0.25f),Quaternion::RotateEuler(0.0f,90.f,0.0f)); // 囲われた宝箱

	stage3.CreateBlock(XMFLOAT3(-15.5f,-4.0f,-2.0f),XMFLOAT3(5.0f,1.0f,7.0f)); // 始まって右手にある浮遊床
	stage3.CreateBlock(XMFLOAT3(-15.5f,-4.0f,10.0f),XMFLOAT3(3.0f,0.5f,3.0f));
	stage3.CreateKey(XMFLOAT3(-16.5f, -2.0f, 0.0f), Color::EBlue); // 浮遊壁の上のカギ

	stage3.CreateLock(XMFLOAT3(-9.0f,2.0f,-5.0f),Color::ERed); // 浮遊壁からくそでか壁の向こう側に行くことを妨げるロック
	stage3.CreateLock(XMFLOAT3(0.0f,4.0f,-5.0f),Color::EBlue); // くそでか壁の向こう側のロック
	stage3.CreateStair(4,XMFLOAT3(0.0f,2.0f,-5.0f));           // くそでか壁の向こう側の階段
	stage3.CreateBlock(XMFLOAT3(2.0f,6.0f,-5.0f));             // 階段途中にあるブロック
	
	stage3.CreateStatusUI();
	stage3.CreateSkyMap();
}


void Stage4Creater::CreateStage(Stage& stage4)const {
	stage4.CreatePerson(XMFLOAT3(-1.0f,3.0f,0.0f));
	stage4.CreateKaeru(XMFLOAT3(-1.0f,5.0f,0.0f));
	stage4.CreateStatusUI();
	stage4.CreateSkyMap();
	stage4.CreateTreasureBox(XMFLOAT3(-10.0f,15.0f,-23.0f));
	stage4.CreateLock(XMFLOAT3(-10.0f,17.0f,-23.0f),Color::ERed); // 宝の真上のロック
	stage4.CreateBlock(XMFLOAT3(-10.0f,13.0f,-23.0f)); // 宝を乗せるブロック
	stage4.CreateBlock(XMFLOAT3(-10.0f,15.0f,-25.0f),XMFLOAT3(1.0f,3.0f,1.0f));// 宝を守るブロック
	stage4.CreateBlock(XMFLOAT3(-8.0f,15.0f,-23.0f));                          // 宝を守るブロック
	stage4.CreateBlock(XMFLOAT3(-12.0f,15.0f,-23.0f));                         // 宝を守るブロック
	stage4.CreateBarrel(XMFLOAT3(3.0f,20.0f,-15.0f));                      // 上方にある台の上に乗った樽
	stage4.CreateBlock(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(8.0f,1.0f,8.0f)); // 床
	stage4.CreateBlock(XMFLOAT3(3.0f, 17.0f, -14.0f), XMFLOAT3(2.0f, 1.0f, 8.0f)); // 樽を乗せた上方にある床
	stage4.CreateBlock(XMFLOAT3(-10.0f,4.75f,0.0f),XMFLOAT3(1.0f,1.75f,1.0f),L"Asset/Wood.png");     // 支柱
	stage4.CreateBlock(XMFLOAT3(-10.0f, 6.5f, 0.0f),XMFLOAT3(3.0f, 0.5f, 3.0f),L"Asset/Dirt.png");// 支柱の上の台
	stage4.CreateBlock(XMFLOAT3(-11.0f,0.0f,0.0f),XMFLOAT3(3.0f,1.0f,1.0f)); // 初期位置にある床と支柱下にある床をつなぐ床
	stage4.CreateBlock(XMFLOAT3(-17.0f,0.0f,0.0f),XMFLOAT3(3.0f,1.0f,3.0f)); // 支柱下にある樽とか乗せた床
	stage4.CreateBarrel(XMFLOAT3(-17.0f,5.0f,0.0f));                         // 支柱下の床ある樽
	stage4.CreateKey(XMFLOAT3(-14.0f,2.0f,0.0f),Color::ERed); // 支柱下のロックのそばにある鍵
	stage4.CreateLock(XMFLOAT3(-10.0f,8.f,0.0f),Color::ERed); // 支柱台のロック
	stage4.CreateLock(XMFLOAT3(-10.0f,2.0f,0.0f),Color::ERed); // 支柱下のロック
	stage4.CreateBlock(XMFLOAT3(-10.0f, 12.0f, -9.0f), XMFLOAT3(3.0f, 1.0f, 6.0f)); // 支柱台の上の浮遊台
	stage4.CreateBlock(XMFLOAT3(-5.0f, 17.0f, -19.0f),XMFLOAT3(6.0f,1.0f,3.0f)); // 支柱台の上の浮遊台から移れる床
	
}