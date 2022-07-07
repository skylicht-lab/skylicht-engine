Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uColor;
};

float4 main(PS_INPUT input) : SV_TARGET
{	
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	return diffuseMap * uColor;
}