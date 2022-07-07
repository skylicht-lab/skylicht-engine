Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
};

cbuffer cbPerFrame
{
	float4 uColor;
	float4 uLightDirection;
};

float4 main(PS_INPUT input) : SV_TARGET
{	
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	
	float NdotL = max(dot(input.worldNormal, uLightDirection.xyz), 0.0);
	
	return diffuseMap * uColor * NdotL;
}