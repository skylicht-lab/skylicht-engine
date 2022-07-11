Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexRamp : register(t1);
SamplerState uTexRampSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
};

cbuffer cbPerFrame
{
	float4 uLightDirection;
	float4 uLightColor;
	float4 uColor;
	float4 uShadowColor;
	float2 uWrapFactor;
	float3 uSpecular;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 diffuseMap = sRGB(uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0).rgb);
	
	float NdotL = max((dot(input.worldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x), 0.0);
	
	float3 rampMap = uTexRamp.Sample(uTexRampSampler, float2(NdotL, NdotL)).rgb;
	
	float3 color = sRGB(uColor.rgb);
	float3 shadowColor = sRGB(uShadowColor.rgb);
	float3 lightColor = sRGB(uLightColor.rgb);
	
	// Shadows intensity through alpha
	float3 ramp = lerp(color, shadowColor, uColor.a);
	ramp = lerp(ramp, color, rampMap);
	
	// Specular
	float3 h = normalize(uLightDirection.xyz + input.worldViewDir);
	float NdotH = max(0, dot(input.worldNormal, h));
	
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	
	return float4(diffuseMap * lightColor * ramp + lightColor * spec, 1.0);
}