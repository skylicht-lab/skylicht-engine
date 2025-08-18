struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

cbuffer cbPerFrame
{
	float4 uColor;
	float4 uEmissive;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 color = sRGB(input.color.rgb * uColor.rgb) * uEmissive.rgb * uEmissive.a;
	return float4(color, 1.0);
}