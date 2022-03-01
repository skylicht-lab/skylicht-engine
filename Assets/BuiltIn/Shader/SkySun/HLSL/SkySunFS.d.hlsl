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

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 viewDir = normalize(input.worldPos.xyz - uCamPosition.xyz);
	
	float y = 1.0 - (max(viewDir.y, 0.0) * 0.8 + 0.2) * 0.8;

	float3 skyColor = float3(pow(y, 2.0), y, 0.6 + y*0.4) * 1.1;
	float3 groundColor = float3(0.4, 0.4, 0.4);
	
	float3 result = lerp(skyColor, sRGB(groundColor), pow(smoothstep(0.0,-0.025, viewDir.y), 0.2));
	float4 blend = input.color * uColor;
	
	return float4(result * sRGB(blend.rgb), blend.a);
}