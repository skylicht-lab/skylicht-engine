struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float4 worldPos: WORLD_POSITION;
};

cbuffer cbPerFrame
{
	float4 uNoiseOffset;
	float4 uElectricColor;
};

#include "LibNoise.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float rz = fbm(uNoiseOffset.xyz + input.worldPos.xyz * uNoiseOffset.w);
	
	rz *= 2.0f;
	
	float3 col = uElectricColor.rgb / rz;

	return input.color * float4(col, 1.0);
}