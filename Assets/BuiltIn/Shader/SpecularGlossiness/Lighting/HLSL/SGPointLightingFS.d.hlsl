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

#include "LibSolverMetallic.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 baseColor = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float3 light = uTexLight.Sample(uTexLightSampler, input.tex0).xyz;

	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);
	
	float spec = data.r;
	float gloss = data.g;
	
	// Roughness
	float roughness = 1.0 - gloss;

	// Metallic
	float3 f0 = spec;
	float3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	
	// Color
	f0 = float3(0.04, 0.04, 0.04);
	float3 diffuseColor = baseColor.rgb;
	specularColor = lerp(f0, baseColor.rgb, metallic);
	
	// Lighting	
	float3 direction = uLightPosition.xyz - position;
	float distance = length(direction);	
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y));
	
	float3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));
	
	// Specular
	float3 H = normalize(direction + viewDir);	
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	
	float3 lightColor = uLightColor.rgb * (NdotL * attenuation) + specular * specularColor.rgb * attenuation;
	
	return float4(light + lightColor, 1.0);
}