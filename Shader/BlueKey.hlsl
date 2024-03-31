//青いかぎのためのシェーダ、ピクセルシェーダの出力を青色にするだけ。

//共通のコンスタントバッファ。シェーダのインクルードとか上手くできる方法見つけたら改善する
cbuffer global
{
	matrix g_w;//ワールド行列
	matrix g_wvp; //ワールドから射影までの変換行列
	float4 g_eye;	//カメラ（視点）
	float4 g_ambient;//環境光
	float4 g_diffuse;//拡散反射光
	float4 g_specular;//鏡面反射光
    float4 g_lightPos; //ライトの方向ベクトル
};

Texture2D g_Tex: register(t0);
SamplerState g_Sampler : register(s0);

//構造体
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 light : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 eyeVector : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
//バーテックスシェーダー
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = mul(pos, g_wvp);
	output.normal = abs(mul(normal, (float3x3)g_w));
    output.light = g_lightPos;
	float3 posWorld = mul(pos, g_w);
	output.eyeVector = g_eye - posWorld;  //ものから視点への方向ベクトル
	output.uv = uv;

	return output;
}
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 normal = normalize(input.normal); // 法線を正規化
    float3 lightDir = normalize(input.light); // ライトベクトルの正規化
    float3 viewDir = normalize(input.eyeVector); // 視線ベクトルの正規化
    float4 normalLightAngle = dot(normal, lightDir); // 正規化されてるので法線と光ベクトルの内積は、法線と光のなす角
    normalLightAngle = saturate(-1 * normalLightAngle); // より、光が当たっているのはなす角に180度足した結果なので-1を掛ける。
	
    float3 reflect = normalize(2 * normalLightAngle * normal - lightDir); // 反射ベクトル
	//                鏡面反射光 * cos(反射ベクトルと目線へのベクトル)＾輝度
    float4 specular = g_specular * pow(saturate(dot(reflect, viewDir)), 2); // 2は輝度(どのくらいピカっとするか、高ければ高いほどピカル)

    float4 texel;
    texel.x = 0.1f;
    texel.y = 0.1f;
    texel.z = 0.6f;
	// 拡散反射光
    float4 diffuse = g_diffuse * normalLightAngle;
	// 拡散反射光完成 鏡面反射も足して出来上がり
    float4 lighting = diffuse + specular + g_ambient;
    float4 final = lighting * texel;
    final.w = 1.0f;
    return final;
}