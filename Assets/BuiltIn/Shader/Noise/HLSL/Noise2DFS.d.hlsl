struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uNoiseOffset;
};

#include "LibNoise2D.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	// float3 n = noised(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w);
	// n = 0.5 + 0.5*n;	
	// return input.color * float4(n.yzx, 1.0);

	float f = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w);
	f = 0.5 + 0.5*f;
	return input.color * float4(f, f, f, 1.0);
}