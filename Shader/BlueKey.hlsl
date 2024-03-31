//�������̂��߂̃V�F�[�_�A�s�N�Z���V�F�[�_�̏o�͂�F�ɂ��邾���B

//���ʂ̃R���X�^���g�o�b�t�@�B�V�F�[�_�̃C���N���[�h�Ƃ���肭�ł�����@����������P����
cbuffer global
{
	matrix g_w;//���[���h�s��
	matrix g_wvp; //���[���h����ˉe�܂ł̕ϊ��s��
	float4 g_eye;	//�J�����i���_�j
	float4 g_ambient;//����
	float4 g_diffuse;//�g�U���ˌ�
	float4 g_specular;//���ʔ��ˌ�
    float4 g_lightPos; //���C�g�̕����x�N�g��
};

Texture2D g_Tex: register(t0);
SamplerState g_Sampler : register(s0);

//�\����
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 light : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 eyeVector : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
//�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = mul(pos, g_wvp);
	output.normal = abs(mul(normal, (float3x3)g_w));
    output.light = g_lightPos;
	float3 posWorld = mul(pos, g_w);
	output.eyeVector = g_eye - posWorld;  //���̂��王�_�ւ̕����x�N�g��
	output.uv = uv;

	return output;
}
//�s�N�Z���V�F�[�_�[
//
float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 normal = normalize(input.normal); // �@���𐳋K��
    float3 lightDir = normalize(input.light); // ���C�g�x�N�g���̐��K��
    float3 viewDir = normalize(input.eyeVector); // �����x�N�g���̐��K��
    float4 normalLightAngle = dot(normal, lightDir); // ���K������Ă�̂Ŗ@���ƌ��x�N�g���̓��ς́A�@���ƌ��̂Ȃ��p
    normalLightAngle = saturate(-1 * normalLightAngle); // ���A�����������Ă���̂͂Ȃ��p��180�x���������ʂȂ̂�-1���|����B
	
    float3 reflect = normalize(2 * normalLightAngle * normal - lightDir); // ���˃x�N�g��
	//                ���ʔ��ˌ� * cos(���˃x�N�g���Ɩڐ��ւ̃x�N�g��)�O�P�x
    float4 specular = g_specular * pow(saturate(dot(reflect, viewDir)), 2); // 2�͋P�x(�ǂ̂��炢�s�J���Ƃ��邩�A������΍����قǃs�J��)

    float4 texel;
    texel.x = 0.1f;
    texel.y = 0.1f;
    texel.z = 0.6f;
	// �g�U���ˌ�
    float4 diffuse = g_diffuse * normalLightAngle;
	// �g�U���ˌ����� ���ʔ��˂������ďo���オ��
    float4 lighting = diffuse + specular + g_ambient;
    float4 final = lighting * texel;
    final.w = 1.0f;
    return final;
}