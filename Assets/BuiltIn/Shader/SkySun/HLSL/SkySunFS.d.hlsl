struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float4 worldPos: WORLD_POSITION;
};

cbuffer cbPerFrame
{
	float4 uLightDirection;
	float4 uCamPosition;
	float4 uIntensity;
	float4 uAtmospheric;
	float4 uSun;
	float4 uGlare1;
	float4 uGlare2;
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"
#include "LibSkySun.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 viewDir = normalize(input.worldPos.xyz - uCamPosition.xyz);
	
	// SKY & SUN
	float3 skyColor = GetSkyColor(
		viewDir,
		uLightDirection.xyz,
		uIntensity.x,				// intensiy
		uAtmospheric,				// atmospheric, intensity
		uSun, 						// sun, intensiy
		uGlare1,					// glare1, intensity
		uGlare2,					// glare2, intensity
		uIntensity.y				// sun radius
	);
	
	// GROUND
	float3 groundColor = float3(0.4, 0.4, 0.4);
	
	// RESULT
	float3 result = lerp(skyColor, sRGB(groundColor), pow(smoothstep(0.0,-0.025, viewDir.y), 0.2));
	return float4(result, 1.0);
}