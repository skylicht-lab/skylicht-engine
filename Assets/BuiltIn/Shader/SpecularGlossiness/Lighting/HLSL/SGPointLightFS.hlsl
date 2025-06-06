// File Generated by Assets/BuildShader.py - source: [SGPointLightFS.d.hlsl : _]
Texture2D uTexPosition : register(t0);
SamplerState uTexPositionSampler : register(s0);
Texture2D uTexNormal : register(t1);
SamplerState uTexNormalSampler : register(s1);
Texture2D uTexData : register(t2);
SamplerState uTexDataSampler : register(s2);
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
float3 pointlight(
	const float3 position,
	const float3 normal,
	const float3 camPosition,
	const float4 lightColor,
	const float3 lightPosition,
	const float4 lightAttenuation,
	const float spec,
	const float gloss,
	const float3 specColor)
{
	float3 direction = lightPosition - position;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * lightAttenuation.y)) * lightColor.a;
	float3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));
	float3 v = camPosition - position;
	float3 viewDir = normalize(v);
	float3 H = normalize(direction + viewDir);
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0 * gloss) * spec;
	return (lightColor.rgb * NdotL + specular * specColor) * attenuation;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float3 lightColor = pointlight(
		position,
		normal,
		uCameraPosition.xyz,
		uLightColor,
		uLightPosition.xyz,
		uLightAttenuation,
		data.r,
		data.g,
		float3(1.0, 1.0, 1.0));
	return float4(lightColor, 1.0);
}
