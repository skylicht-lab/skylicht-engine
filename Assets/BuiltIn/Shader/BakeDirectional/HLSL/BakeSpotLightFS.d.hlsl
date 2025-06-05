TextureCube uPointLightShadowMap : register(t0);
SamplerState uPointLightShadowMapSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldPosition: WORLDPOSITION;
	float3 tex0 : LIGHTMAP;
};

cbuffer cbPerFrame
{
	float4 uLightPosition;
	float4 uLightDirection;
	float4 uLightAttenuation;
	float4 uLightColor;
};

#include "../../Light/HLSL/LibSpotLightShadow.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 directionalLightColor = spotlightShadow(
		input.worldPosition, 
		input.worldNormal,
		float3(0.0, 100.0, 0.0), 
		uLightColor, 
		uLightPosition.xyz, 
		uLightAttenuation, 
		0.0, 
		0.1,
		float3(1.0, 1.0, 1.0));
		
	return float4(directionalLightColor / 3.0, 1.0);
}