#pragma once
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

//--------------------------------------------------------------------------
/// [�@�\]�@�������̈ʒu�ɂ���I�u�W�F�N�g����ʓ��ɂ���̂��𔻒肵�܂�
//--------------------------------------------------------------------------
bool VFCulling(const DirectX::XMFLOAT3& pos, const DirectX::XMMATRIX& pmView, float Angle, float NearClip, float FarClip, float Aspect); // ������

enum class Shape {
	Cube,
	Square
};


//=========================================================================================================================
/// [�@�\] ���f���Ƃ�1�ȏ�̃��b�V���̉�ł���A�Ƃ��̃v���O�������ł͒�`����B�����`�悵����A�e�N�X�`����ς�����ł���
//=========================================================================================================================
class Model{
public:
	static void Init(std::shared_ptr<class Renderer> renderer);

	//----------------------------------------------------------------
	/// [�@�\] 3D���f���t�@�C�������ƂɃ��f�����`������
	///        ���_�V�F�[�_�ƃs�N�Z���V�F�[�_���ʃt�@�C���̎��Ɏg��
	//----------------------------------------------------------------
	Model(const wchar_t* modelFileName,const std::wstring& vertexShaderName,const std::wstring& pixelSaderName);
	//-----------------------------------------------------------------
	/// [�@�\] 3D���f���t�@�C�������ƂɃ��f�����`������
	///        ���_�V�F�[�_�ƃs�N�Z���V�F�[�_������t�@�C���̎��Ɏg��
	//-----------------------------------------------------------------
	Model(const wchar_t* modelFileName,const std::wstring& shaderName);
	//------------------------------------------------------------------
	/// [�@�\] Shape�񋓌^�̌`��̃��f�����`������B
	///        ���_�V�F�[�_�ƃs�N�Z���V�F�[�_���ʃt�@�C���̎��Ɏg��
	//------------------------------------------------------------------
	Model(Shape shape, const std::wstring& textureName,const std::wstring& vertexShaderName,const std::wstring& pixelShaderName);
	//-------------------------------------------------------------------
	/// [�@�\] Shape�񋓌^�̌`��̃��f�����`������
	///        ���_�V�F�[�_�ƃs�N�Z���V�F�[�_������t�@�C���̎��Ɏg��
	//-------------------------------------------------------------------
	Model(Shape shape, const std::wstring& textureName,const std::wstring& shaderName);


	~Model();

	Model(Model&&);
	Model& operator=(Model&&);

	
	//-------------------------------------------------------
	/// [�@�\] ���̃t���[���ł̕`��J�n���Ɉ�x�����Ăяo��
	//-------------------------------------------------------
	static void BeginRender();

	//-----------------------------------------------------------------
	// [�@�\] ���̃t���[���ł̕`�悪�S�ďI������Ƃ��Ɉ�x�����Ăяo��
	//-----------------------------------------------------------------
	static void EndRender();

	//-----------------------------------------------------
	/// [�@�\] ���[���h�}�g���N�X�ƃJ�����ʒu�����Ƃɕ`��
	//-----------------------------------------------------
	void Render(const DirectX::XMMATRIX& worldMat,const class Camera& camera);

	//--------------------------------------------------------------------------------------------------------------------
	/// [�@�\] �e�����e�N�X�`���ɏ������ށB
	/// [����] �e��`�悷��킯�ł͂Ȃ��B�S�Ẳe���e�N�X�`���ɏ������񂾂̂��Ƀ����_�[���Ăяo���Ɛ������e���������
	//--------------------------------------------------------------------------------------------------------------------
	void ShadowMap(const DirectX::XMMATRIX& worldMat,const class Camera& camera);

	//---------------------------------------------------------------
	/// [�@�\] ���̃��f���������̃��b�V������\������Ă��邩���ׂ�
	//---------------------------------------------------------------
	bool ConsistsMultipleMesh();

	//-------------------------------------------------------------
	/// [�@�\] �e�N�X�`����ύX����B
	/// [����1] �e�N�X�`����
	/// [����2] ���f�����\������A���Ԗڂ̃��b�V�����g�p����̂�
	//-------------------------------------------------------------
	void ChangeTexture(const std::wstring& textureName,int meshIndex);

	//-------------------------------------------------------------
	/// [�@�\] 0�Ԗڂ̃��b�V���̃e�N�X�`����ύX����B
	/// [����1] �e�N�X�`����
	//-------------------------------------------------------------
	void ChangeTexture(const std::wstring& textureName);

private:
	std::vector<class MyMesh> mMeshes;  // ���f���̃��b�V��
	std::shared_ptr<class PipelineState> mPipelineState;
	static std::unique_ptr<class PipelineManager> mPipelineManager;
	static std::shared_ptr<class Renderer> mRenderer;
	static bool mWasInstanced;

	// �R�s�[�͏d���̂ŕ��󂵂Ă���
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;
};



using ModelPtrS = std::shared_ptr<Model>;

