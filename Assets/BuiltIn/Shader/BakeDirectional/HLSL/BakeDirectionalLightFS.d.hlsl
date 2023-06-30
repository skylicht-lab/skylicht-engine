Texture2D uShadowMap : register(t0);
SamplerState uShadowMapSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldPosition: WORLDPOSITION;
	float3 tex0 : LIGHTMAP;
};

cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uLightDirection;
	float4x4 uShadowMatrix;
};

#include "../../Shadow/HLSL/LibShadowSimple.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 shadowCoord = mul(float4(input.worldPosition, 1.0), uShadowMatrix);
	float visibility = shadowSimple(shadowCoord);

	float NdotL = max(dot(input.worldNormal, uLightDirection.xyz), 0.0);
	float3 directionalLightColor = NdotL * uLightColor.rgb / 3.0;
	return float4(directionalLightColor * visibility, visibility);
}