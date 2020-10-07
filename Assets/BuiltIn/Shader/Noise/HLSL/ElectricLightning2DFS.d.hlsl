struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uNoiseOffset;
	float4 uElectricColor;
};

#include "LibNoise2D.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	float f = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w);
	float t = 0.2;

	// convert uvx [0 - 1] -> x [-1, 1] to claim lighting center	
	float x = input.tex0.x * 2.0 - 1.0;

	f = abs(f * t + x + 0.01);
	f = pow(f, 0.2);

	// f = f * 2.0
	// float3 col = uElectricColor.rgb / f;

	float3 col = float3(1.7, 1.7, 1.7);
	col = col * -f + col;
	col = col * col;
	col = col * col;
	col = col * uElectricColor.rgb;

	return input.color * float4(col, 1.0);
}