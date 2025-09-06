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
	float3 uLightDirX;
	float3 uLightDirY;
	float2 uLightSize;
	float4 uLightColor;
};

#include "../../Light/HLSL/LibAreaLight.hlsl"

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

	float3 directionalLightColor = arealight(
		input.worldPosition, 
		worldNormal,
		float3(0.0, 100.0, 0.0), 
		uLightColor, 
		uLightPosition.xyz, 
		uLightDirX, 
		uLightDirY,
		uLightSize);
		
	return float4(directionalLightColor / 3.0, 1.0);
}