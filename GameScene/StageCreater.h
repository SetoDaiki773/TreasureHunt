#pragma once

//===================================================================================================================
/// [機能] ステージを作成する関数を提供する。stageクラスにはオブジェクト作成関数があるので、それを元に作っていく
//===================================================================================================================
class StageCreater {
public:
	//--------------------------------------------------------------------
	/// [機能] 引数のステージにオブジェクトなどを配置してステージ1を作る
	//--------------------------------------------------------------------
	virtual void CreateStage(class Stage& stage)const;
};

class Stage2Creater : public StageCreater {
	//--------------------------------
	/// [機能] ステージ2をつくる
	//--------------------------------
	void CreateStage(class Stage& stage)const override;
};

class Stage3Creater : public StageCreater {
	//--------------------------------
	/// [機能] ステージ3をつくる
	//--------------------------------
	void CreateStage(class Stage& stage)const override;
};

class Stage4Creater : public StageCreater {
	//---------------------------------
	/// [機能] ステージ4をつくる
	//---------------------------------
	void CreateStage(class Stage& stage)const override;
};

class Stage5Creater : public StageCreater {
	//---------------------------------
	// [機能] ステージ5をつくる
	//---------------------------------
	void CreateStage(class Stage& stage)const override;

};