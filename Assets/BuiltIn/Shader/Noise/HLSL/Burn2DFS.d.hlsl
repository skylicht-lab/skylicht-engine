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
	int ITERATIONS = 13;
	
	float uSPEED = 10.0;
	float uDISPLACEMENT = 0.05;
	float uTIGHTNESS = 10.0;
	float uYOFFSET = 0.3;
	float uYSCALE = 0.2;
	float uXSCALE = 0.5;
	float3 uFLAMETONE = float3(50.0, 5.0, 1.0);
	
	float nx = 0.0;
	float ny = 0.0;
	
	float time = uNoiseOffset.x;
	float2 uv = input.tex0;
	float2 uvNoise = input.tex0 * float2(uXSCALE, uYSCALE);
	
	for (int i=4; i<ITERATIONS+1; i++)
	{
		float ii = pow((float)i, 2.0);
		float ifrac = float(i)/float(ITERATIONS);
		
		float t = ifrac * time * uSPEED;
		float d = (1.0-ifrac) * uDISPLACEMENT;
		
		nx += snoise( float3(uvNoise.x*ii-time*ifrac, uvNoise.y*ii-t, 0.0)) * d * 2.0;
		ny += snoise( float3(uvNoise.x*ii+time*ifrac, uvNoise.y*ii-t, time*ifrac/ii)) * d;
	}
	
	float2 pos = float2(uv.x+nx, uv.y+ny);	
	
	// float flame = clamp(1.0 - pos.y+uYOFFSET, 0.0, 1.0);
	float flame = clamp(sin(pos.x*3.1416) - pos.y+uYOFFSET, 0.0, 1.0);
	
	float3 col = pow(flame, uTIGHTNESS) * uFLAMETONE;
    
    // tonemapping
    col = col / (1.0+col);
	float e = 1.0/2.2;
    col = pow(col, float3(e, e, e));
    col = clamp(col, 0.0, 1.0);
	
	return input.color * float4(col, 1.0);
}