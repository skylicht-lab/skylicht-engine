struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float4 worldPos: WORLD_POSITION;
};

#include "LibNoise.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float n = pnoise(input.worldPos.xyz * 8.0);
	
	return input.color * float4(n, n, n, 1.0);
}