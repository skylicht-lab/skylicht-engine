Texture2D uTexColor : register(t0);
SamplerState uTexColorSampler : register(s0);
Texture2D uTexLuminance : register(t1);
SamplerState uTexLuminanceSampler : register(s1);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};
static const float gamma = 2.2;
static const float invGamma = 1.0/2.2;
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}
float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 color = uTexColor.Sample(uTexColorSampler, input.tex0);
	float avgLuminance = uTexLuminance.SampleLevel(uTexLuminanceSampler, input.tex0, 10.0).x;
	float target = 0.2;
	float threshold = 2.5;
	float linearExposure = max((target / avgLuminance), 0.0001);
	float exposure = min(exp2(log2(linearExposure)), threshold);
	return float4(linearRGB(exposure * color.rgb), color.a);
}
