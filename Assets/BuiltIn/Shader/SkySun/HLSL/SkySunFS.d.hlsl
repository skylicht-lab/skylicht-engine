struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float4 worldPos: WORLD_POSITION;
};

cbuffer cbPerFrame
{
	float4 uColor;
	float2 uIntensity;
	float4 uLightDirection;
	float4 uCamPosition;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"
#include "LibSkySun.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 viewDir = normalize(input.worldPos.xyz - uCamPosition.xyz);	
	float y = 1.0 - (max(viewDir.y, 0.0) * 0.8 + 0.2) * 0.8;
	
	// SKY & SUN
	float3 skyColor = GetSkyColor(
		viewDir,
		uLightDirection.xyz,
		1.1,						// intensiy
		float4(1.0, 0.8, 0.7, 0.1),	// atmospheric, intensiy
		float4(1.0, 0.6, 0.1, 0.5), // sun, intensiy
		float4(1.0, 0.6, 0.1, 0.4),	// glare1, intensiy
		float4(1.0, 0.4, 0.2, 0.2),	// glare2, intensiy
		800.0						// sun radius
	);
	
	// GROUND
	float3 groundColor = float3(0.4, 0.4, 0.4);
	
	// RESULT
	float3 result = lerp(skyColor, sRGB(groundColor), pow(smoothstep(0.0,-0.025, viewDir.y), 0.2));
	float4 blend = input.color * uColor;
	
	return float4(result * sRGB(blend.rgb), blend.a);
}