Texture2D uDirectionalLight : register(t0);
SamplerState uDirectionalLightSampler : register(s0);

Texture2D uPointLight : register(t1);
SamplerState uPointLightSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 l1 = uDirectionalLight.Sample(uDirectionalLightSampler, input.tex0);
	float4 l2 = uPointLight.Sample(uPointLightSampler, input.tex0);
	return float4(l1.rgb + l2.rgb, l1.a);
}