TextureCube uTexReflect : register(t0);
SamplerState uTexReflectSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float3 worldNormal: WORLDNORMAL;
};

#include "../../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 result = float4(sRGB(uTexReflect.SampleLevel(uTexReflectSampler, input.worldNormal, 0).rgb), 1.0);
	return result;
}