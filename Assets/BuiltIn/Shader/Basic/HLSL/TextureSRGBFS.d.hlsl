Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uColor;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 result = input.color * uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0) * uColor;
	return float4(sRGB(result.rgb), result.a);
}