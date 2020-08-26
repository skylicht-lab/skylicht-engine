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

Texture2DArray uShadowMap : register(t6);
SamplerState uShadowMapSampler : register(s6);

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
	float3 uShadowDistance;
	float4x4 uShadowMatrix[3];
};

#include "LibShadow.hlsl"
#include "LibSG.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 albedo = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float4 light = uTexLight.Sample(uTexLightSampler, input.tex0);
	float3 indirect = uTexIndirect.Sample(uTexIndirectSampler, input.tex0).rgb;

	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);
	
	float directMul = uLightMultiplier.x;
	float indirectMul = uLightMultiplier.y;
	float lightMul = uLightMultiplier.z;
	
	// backface when render lightmap
	if (dot(viewDir, normal) < 0)
	{
		normal = normal * -1.0;
		directMul = 0.0;
		indirectMul = 0.0;
		lightMul = 0.0;
	}
	
	// shadow
	float depth = length(v);
		
	float4 shadowCoord[3];
	shadowCoord[0] = mul(float4(position, 1.0), uShadowMatrix[0]);
	shadowCoord[1] = mul(float4(position, 1.0), uShadowMatrix[1]);
	shadowCoord[2] = mul(float4(position, 1.0), uShadowMatrix[2]);
	
	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;
	float visibility = shadow(shadowCoord, shadowDistance, depth);
	
	// lighting
	float3 color = SG(
		albedo,
		data.r,
		data.g,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb,
		visibility,
		light,
		indirect,
		directMul,
		indirectMul,
		lightMul);
	
	return float4(color, 1.0);
}