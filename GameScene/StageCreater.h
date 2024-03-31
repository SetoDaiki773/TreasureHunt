#pragma once

//===================================================================================================================
/// [�@�\] �X�e�[�W���쐬����֐���񋟂���Bstage�N���X�ɂ̓I�u�W�F�N�g�쐬�֐�������̂ŁA��������ɍ���Ă���
//===================================================================================================================
class StageCreater {
public:
	//--------------------------------------------------------------------
	/// [�@�\] �����̃X�e�[�W�ɃI�u�W�F�N�g�Ȃǂ�z�u���ăX�e�[�W1�����
	//--------------------------------------------------------------------
	virtual void CreateStage(class Stage& stage)const;
};

class Stage2Creater : public StageCreater {
	//--------------------------------
	/// [�@�\] �X�e�[�W2������
	//--------------------------------
	void CreateStage(class Stage& stage)const override;
};

class Stage3Creater : public StageCreater {
	//--------------------------------
	/// [�@�\] �X�e�[�W3������
	//--------------------------------
	void CreateStage(class Stage& stage)const override;
};

class Stage4Creater : public StageCreater {
	//---------------------------------
	/// [�@�\] �X�e�[�W4������
	//---------------------------------
	void CreateStage(class Stage& stage)const override;
};

class Stage5Creater : public StageCreater {
	//---------------------------------
	// [�@�\] �X�e�[�W5������
	//---------------------------------
	void CreateStage(class Stage& stage)const override;

};