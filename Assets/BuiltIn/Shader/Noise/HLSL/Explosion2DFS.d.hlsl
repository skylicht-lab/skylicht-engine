struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;	
};

cbuffer cbPerFrame
{
	float4 uNoiseOffset;
	float4 uExplosionParams;
};

#include "LibNoise2D.hlsl"

float3 heatToColor(float heat)
{
    float3 col = lerp(
		float3(0.0, 0.0, 0.0), 
		float3(1.0, 0.3, 0.0),
		clamp(heat * 15.0 - 2.0,   0.0, 1.0));
	
    col = lerp(col, 
		float3(1.0, 1.0, 0.6), 
		clamp(heat * 15.1 - 4.0,   0.0, 1.0));
		
    col = lerp(col, 
		float3(1.0, 0.9, 0.8), 
		clamp(heat * 190.0 - 60.0, 0.0, 1.0));
		
    return col;
}

float4 main(PS_INPUT input) : SV_TARGET
{		
	float f = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w);	
	float t = pnoise(uNoiseOffset.xy + input.tex0 * uNoiseOffset.w * 1.3);
	
	f = 0.5 + 0.5*f;
	t = 0.5 + 0.5*t;
	
	float a = f;
	f = f * uExplosionParams.x;
	
	float heat = f * (0.4 - (1.0 - t) * 0.2);
	
	float3 color = heatToColor(heat);
	float3 scolor = float3(0.0, 0.0, 0.0);
		
	float smoke = heat/0.03;
	if (smoke < 1.0)
	{
		float s = smoke*0.5;
		scolor = float3(s, s, s);
	}
	
	color = lerp(color, scolor, f);
	
	color = smoothstep(
		float3(0.0, 0.0, 0.0), 
		float3(0.3, 0.3, 0.3), 
		color);
	
	a = a * uExplosionParams.y;	
	a = smoothstep(0.0, 0.95, a);
	
	return input.color * float4(color, a);
}