// ���C�e�B���O�����Ȃ��B3D�I�u�W�F�N�g�p�B

//���ʂ̃R���X�^���g�o�b�t�@�B�V�F�[�_�̃C���N���[�h�Ƃ���肭�ł�����@����������P����
cbuffer global: register(b0)
{
    matrix g_w; //���[���h�s��
    matrix g_wvp; //���[���h����ˉe�܂ł̕ϊ������s��
    float4 g_lightpos; //�|�C���g���C�g
    float4 g_eye; //�J�����ʒu
    float4 g_ambient; //����
    float4 g_diffuse; //�g�U���ˌ�
    float4 g_specular; //���ʔ��ˌ�

};
Texture2D g_tex: register(t0);
SamplerState g_sampler : register(s0);

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//
//
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(pos, g_wvp);
	output.uv = tex;

	return output;
}
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	// ���C�e�B���O�����e�N�X�`���J���[�����̂܂ܕԂ��B
	float4 texel = g_tex.Sample(g_sampler, input.uv);
	return texel;
}