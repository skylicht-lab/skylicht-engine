Texture2D uShadowMap : register(t0);
SamplerState uShadowMapSampler : register(s0);

#if defined(NORMAL_MAP)	
Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);
#endif

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldPosition: WORLDPOSITION;
	float2 tex0: TEXCOORD0;
#if defined(NORMAL_MAP)	
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
#endif
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

#if defined(NORMAL_MAP)
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 worldNormal = normalize(mul(localCoords, rotation));
#else
	float3 worldNormal = input.worldNormal;
#endif

	float NdotL = max(dot(worldNormal, uLightDirection.xyz), 0.0);
	float3 directionalLightColor = NdotL * uLightColor.rgb / 3.0;
	return float4(directionalLightColor * visibility, visibility);
}