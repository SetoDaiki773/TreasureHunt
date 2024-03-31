// UI用。ForUIシェーダと違って、白色も描画する。

//共通のコンスタントバッファ。シェーダのインクルードとか上手くできる方法見つけたら改善する
cbuffer global
{
	matrix g_w;//ワールド行列
	matrix g_wvp; //ワールドから射影までの変換行列
	float4 g_lightPos;//ライトの方向ベクトル
	float4 g_eye;	//カメラ（視点）
	float4 g_ambient;//環境光
	float4 g_diffuse;//拡散反射光
	float4 g_specular;//鏡面反射光
};

Texture2D g_Tex: register(t0);
SamplerState g_Sampler : register(s0);

//構造体
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD3;
};
//バーテックスシェーダー
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

    output.pos = mul(pos,g_w); // UIようなのでワールド変換のみ
	output.uv = uv;

	return output;
}
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 texel = g_Tex.Sample(g_Sampler, input.uv);
	
    // 普通のUI用シェーダはほぼ白はdiscardするけど、このシェーダでは捨てない
	
    return texel;
}