Texture2D uTexColor : register(t0);
SamplerState uTexColorSampler : register(s0);

Texture2D uTexCurrentLum : register(t1);
SamplerState uTexCurrentLumSampler : register(s1);

Texture2D uTexLastLum : register(t2);
SamplerState uTexLastLumSampler : register(s2);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

#include "LibToneMapping.hlsl"

float CalcLuminance(float3 color)
{
    return max(dot(color, float3(0.299, 0.587, 0.114)), 0.0001);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = uTexColor.Sample(uTexColorSampler, input.tex0);
	
	float currentLum = uTexCurrentLum.SampleLevel(uTexCurrentLumSampler, input.tex0, 10.0).x;
	// float lastLum = uTexLastLum.SampleLevel(uTexLastLumSampler, input.tex0, 10.0).x;
	
	if (input.tex0.x < 0.5)
	{		
		float c = currentLum;
		return float4(c, c, c, 1.0);
	}	
	
	/*
	float adaptedLum = lastLum + (currentLum - lastLum) * 0.1;
	float pixelLuminance = CalcLuminance(color.rgb);
	
	float avgLuminance = max(adaptedLum, 0.001f);
	
	float keyValue = 0.2;
	
	float linearExposure = (keyValue / avgLuminance);
	
	float exposure = log2(max(linearExposure, 0.0001f));
    // exposure -= threshold;
    
	color = exp2(exposure) * color;
	*/
	
	return float4(linearRGB(color.rgb), color.a);
}