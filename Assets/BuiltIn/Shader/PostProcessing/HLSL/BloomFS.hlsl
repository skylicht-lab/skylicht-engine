Texture2D uSourceTex : register(t0);
SamplerState uSourceTexSampler : register(s0);

Texture2D uBlurTex : register(t1);
SamplerState uBlurTexSampler : register(s1);

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

float4 main(PS_INPUT input) : SV_TARGET
{
	// base color
	float3 base = uSourceTex.Sample(uSourceTexSampler, input.tex0).rgb;

	// blur color
	float3 blur = uBlurTex.Sample(uBlurTexSampler, input.tex0).rgb;

	float intensity = 1.8;
	float3 m = base + blur * intensity;

	return float4(m, 1.0);
}