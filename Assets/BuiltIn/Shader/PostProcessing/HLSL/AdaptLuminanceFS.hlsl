Texture2D uTexLuminance : register(t0);
SamplerState uTexLuminanceSampler : register(s0);

Texture2D uTexLastLuminance : register(t1);
SamplerState uTexLastLuminanceSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float2 uTimeStep;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float lum = uTexLuminance.Sample(uTexLuminanceSampler, input.tex0).x;
	float lastLum = uTexLastLuminance.Sample(uTexLastLuminanceSampler, input.tex0).x;
	
	float adaptLum = lastLum + (lum - lastLum) * uTimeStep.x * 0.001;
	
	return float4(adaptLum, 0, 0, 0);
}