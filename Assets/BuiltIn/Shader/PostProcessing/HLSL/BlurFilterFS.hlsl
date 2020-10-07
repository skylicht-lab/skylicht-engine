Texture2D uSourceTex : register(t0);
SamplerState uSourceTexSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float2 uTexelSize;
};

float3 upsampleFilter(const float2 uv)
{
	float4 d = float4(-uTexelSize.x, -uTexelSize.y, uTexelSize.x, uTexelSize.y);

	float3 s0 = uSourceTex.SampleLevel(uSourceTexSampler, uv, 0).rgb;
	float3 s1 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.xy, 0).rgb;
	float3 s2 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.zy, 0).rgb;
	float3 s3 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.xw, 0).rgb;
	float3 s4 = uSourceTex.SampleLevel(uSourceTexSampler, uv + d.zw, 0).rgb;

	return (s0 + s1 + s2 + s3 + s4) * 0.2;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 m = upsampleFilter(input.tex0);
	return float4(m, 1.0);
}