Texture2D uTexPosition : register(t0);
SamplerState uTexPositionSampler : register(s0);

Texture2D uTexNormal : register(t1);
SamplerState uTexNormalSampler : register(s1);

Texture2D uTexData : register(t2);
SamplerState uTexDataSampler : register(s2);

#ifdef SHADOW
TextureCube uPointLightShadowMap : register(t3);
SamplerState uPointLightShadowMapSampler : register(s3);
#endif

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightPosition;
	float4 uLightAttenuation;
	float4 uLightColor;
};

#ifdef SHADOW
#include "../../../Light/HLSL/LibPointLightShadow.hlsl"
#else
#include "../../../Light/HLSL/LibPointLight.hlsl"
#endif

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;

#ifdef SHADOW
	float3 lightColor = pointlightShadow(
#else
	float3 lightColor = pointlight(
#endif
		position, 
		normal,
		uCameraPosition.xyz, 
		uLightColor, 
		uLightPosition.xyz, 
		uLightAttenuation, 
		data.r, // spec
		data.g, // gloss 
		float3(1.0, 1.0, 1.0));
	
	return float4(lightColor, 1.0);
}