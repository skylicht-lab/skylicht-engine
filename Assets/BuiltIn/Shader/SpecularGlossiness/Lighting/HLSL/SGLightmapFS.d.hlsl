Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);

Texture2D uTexPosition : register(t1);
SamplerState uTexPositionSampler : register(s1);

Texture2D uTexNormal : register(t2);
SamplerState uTexNormalSampler : register(s2);

Texture2D uTexData : register(t3);
SamplerState uTexDataSampler : register(s3);

Texture2D uTexLight : register(t4);
SamplerState uTexLightSampler : register(s4);

Texture2D uTexIndirect : register(t5);
SamplerState uTexIndirectSampler : register(s5);

#if defined(ENABLE_SSR)
Texture2D uTexLastFrame : register(t7);
SamplerState uTexLastFrameSampler : register(s7);
#endif

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uLightColor;
	float3 uLightMultiplier;
#if defined(ENABLE_SSR)
	float4x4 uView;
	float4x4 uProjection;
#endif
};

#include "LibSGLM.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 albedo = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	
	float4 posdepth = uTexPosition.Sample(uTexPositionSampler, input.tex0);
	float3 position = posdepth.xyz;
	
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float4 light = uTexLight.Sample(uTexLightSampler, input.tex0);
	float3 indirect = uTexIndirect.Sample(uTexIndirectSampler, input.tex0).rgb;

	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);

	float directMul = uLightMultiplier.x;
	float indirectMul = uLightMultiplier.y;
	float lightMul = uLightMultiplier.z;

	// lighting
	float3 color = SGLM(
		albedo,
		data.r,
		data.g,
		posdepth,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb * uLightColor.a,
		light,
		indirect,
		directMul,
		indirectMul,
		lightMul);

	return float4(color, 1.0);
}