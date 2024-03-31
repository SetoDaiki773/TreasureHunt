// UI用のシェーダ。rbgの総和がほぼ3なら描画しない。UIなのでワールド変換のみ適用する

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

    output.pos = mul(pos,g_w); // UIなのでワールド変換のみ
	output.uv = uv;

	return output;
}
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 texel = g_Tex.Sample(g_Sampler, input.uv);
    float rgbTotal = texel.x + texel.y + texel.z;
    if (rgbTotal >= 2.99f)  // 2.99以上(ほぼ白なら)
    {
        discard; // 透明にしちゃう
    }
	
    return texel;
}