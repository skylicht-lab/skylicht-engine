Texture2D uTexture : register(t0);
SamplerState uTextureSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = uTexture.Sample(uTextureSampler, input.tex0);
	float3 result = sRGB(color.rgb * input.color.rgb);
	
	return float4(result, color.a * input.color.a);
}