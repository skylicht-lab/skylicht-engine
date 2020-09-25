// https://www.shadertoy.com/view/lsSGWw

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

#include "LibNoise.hlsl"

float snoise( in float3 x )
{
	return noise(x) * 2.0 - 1.0;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float color = 1.0 - input.tex0.y + 0.4;
    
    float3 coord = float3(input.tex0.x/6.0, input.tex0.y/6.0, 0.5);
	
    coord = 1.0 - coord;
    
	float speedMul = 0.05;
	float3 noiseOffset = float3(0.0, uNoiseOffset.x, uNoiseOffset.x) * speedMul;
	
    int i = 1;
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	
    color = 1.0 - color;
    color *= 2.7;
    
	float3 col = float3(color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.0)*0.15);
	
	return input.color * float4(col, 1.0);
}