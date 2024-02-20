Texture2DArray uTexLightmap : register(t0);
SamplerState uTexLightmapSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 tex0 : LIGHTMAP;
};

cbuffer cbPerFrame
{
	float uLightmapIndex;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 uv = input.tex0;
	uv.z += uLightmapIndex;
	float4 lightColor = uTexLightmap.Sample(uTexLightmapSampler, uv);
	float4 result = float4(lightColor.rgb * 3.0, lightColor.a);
	
	return result;
}