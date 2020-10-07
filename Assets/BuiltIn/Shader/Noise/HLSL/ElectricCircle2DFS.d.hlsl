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

float paintCircle(float2 uv, float2 center, float rad) {

	float2 diff = center - uv;
	float len = length(diff);

	return smoothstep(rad, 0.0, len);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float f = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w);
	float t = 0.2;

	float radius = 0.7;
	float2 center = float2(0.5, 0.5);

	float c = (1.0 - paintCircle(input.tex0, center, radius));

	// convert to [-1, 1]
	c = -1.0 + c * 2.0;

	f = abs(f * t + c + 0.01) * 0.3;
	f = pow(f, 0.2);

	float3 col = float3(1.7, 1.7, 1.7);
	col = col * -f + col;
	col = col * col;
	col = col * col;
	col = col * uElectricColor.rgb;

	return input.color * float4(col, 1.0);
}