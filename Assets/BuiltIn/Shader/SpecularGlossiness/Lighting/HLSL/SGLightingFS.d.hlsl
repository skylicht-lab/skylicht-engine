Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);

Texture2D uTexPosition : register(t1);
SamplerState uTexPositionSampler : register(s1);

Texture2D uTexNormal : register(t2);
SamplerState uTexNormalSampler : register(s2);

Texture2D uTexData : register(t3);
SamplerState uTexDataSampler : register(s3);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;	
};

cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uAmbientLightColor;
	float4 uLightColor;
};

#include "LibSG.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 albedo = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data	= uTexData.Sample(uTexDataSampler, input.tex0).xyz;

	float3 v = uCameraPosition.xyz - position;
	float3 viewDir 	= normalize(v);
	
	float3 color = SG(
		albedo,
		data.r,
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uAmbientLightColor.rgb,
		uLightColor.rgb);
	
	return float4(color, 1.0);
}