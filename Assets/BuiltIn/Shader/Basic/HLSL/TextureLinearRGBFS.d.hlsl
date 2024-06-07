Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 texColor = uTexDiffuse.SampleLevel(uTexDiffuseSampler, input.tex0, 0);
	return float4(input.color.rgb * linearRGB(texColor.rgb), texColor.a * input.color.a);
}