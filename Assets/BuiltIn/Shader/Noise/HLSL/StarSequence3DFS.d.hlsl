// Base on https://www.shadertoy.com/view/4dXGR4

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
#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"

float snoise(float3 coord)
{
	return 1.0 - noise(coord) * 2.0;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 coord = input.worldPos.xyz * uNoiseOffset.w;
	float n = pnoise(uNoiseOffset.xyz + input.worldPos.xyz * 12.0);

	coord += float3(n, 0.0, n);

	float color = 0.5;
	float scale = 1.3;

	int i = 1;
	{
		float power = pow(2.0, i);
		float res = power * scale;
		color += (0.4 / power) * snoise(coord * res);
	}
	i++;
	{
		float power = pow(2.0, i);
		float res = power * scale;
		color += (0.4 / power) * snoise(coord * res);
	}

	color *= 2.3;

	float r = color;
	float g = pow(max(color, 0.0),2.0)*0.4;
	float b = pow(max(color, 0.0),3.0)*0.15;

	float4 ret = input.color * float4(r, g, b, 1.0);
	return float4(sRGB(ret.rgb), ret.a);
}