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

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 sum = float3(0.0, 0.0, 0.0);
	
	float weights[5] = {0.05, 0.25, 0.4, 0.25, 0.05};

	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, -2.0 * uTexelSize.y), 0).rgb * weights[0];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, -1.0 * uTexelSize.y), 0).rgb * weights[1];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0, 0).rgb * weights[2];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, 1.0 * uTexelSize.y), 0).rgb * weights[3];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, 2.0 * uTexelSize.y), 0).rgb * weights[4];
	
	/*
	float weights[9] = {0.05, 0.09, 0.12, 0.15, 0.16, 0.15, 0.12, 0.09, 0.05};

	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, -4.0 * uTexelSize.y), 0.0).rgb * weights[0];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, -3.0 * uTexelSize.y), 0.0).rgb * weights[1];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, -2.0 * uTexelSize.y), 0.0).rgb * weights[2];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, -1.0 * uTexelSize.y), 0.0).rgb * weights[3];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0, 0.0).rgb * weights[4];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, 1.0 * uTexelSize.y), 0.0).rgb * weights[5];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, 2.0 * uTexelSize.y), 0.0).rgb * weights[6];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, 3.0 * uTexelSize.y), 0.0).rgb * weights[7];
	sum += uSourceTex.SampleLevel(uSourceTexSampler, input.tex0 + float2(0.0, 4.0 * uTexelSize.y), 0.0).rgb * weights[8];
	*/
	
	return float4(sum, 1.0);
}