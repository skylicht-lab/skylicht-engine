Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexTiling : register(t1);
SamplerState uTexTilingSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
};

cbuffer cbPerFrame
{
	float4 uColor;
};

float4 main(PS_INPUT input) : SV_TARGET
{	
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	float4 tilingMap = uTexTiling.Sample(uTexTilingSampler, input.tex1);
	return float4(tilingMap.rgb * uColor.rgb * input.color.rgb, diffuseMap.a);
}