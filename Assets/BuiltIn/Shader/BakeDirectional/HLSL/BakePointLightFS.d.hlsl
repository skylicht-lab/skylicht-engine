TextureCube uPointLightShadowMap : register(t0);
SamplerState uPointLightShadowMapSampler : register(s0);

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
	float4 uLightPosition;
	float4 uLightAttenuation;
	float4 uLightColor;
};

#include "../../Light/HLSL/LibPointLightShadow.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
#if defined(NORMAL_MAP)
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 worldNormal = normalize(mul(localCoords, rotation));
#else
	float3 worldNormal = input.worldNormal;
#endif

	float3 directionalLightColor = pointlightShadow(
		input.worldPosition, 
		worldNormal,
		float3(0.0, 100.0, 0.0), 
		uLightColor, 
		uLightPosition.xyz, 
		uLightAttenuation, 
		0.0, 
		0.1,
		float3(1.0, 1.0, 1.0));
		
	return float4(directionalLightColor / 3.0, 1.0);
}