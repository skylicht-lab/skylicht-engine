Texture2DArray uTexLightmap : register(t0);
SamplerState uTexLightmapSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 tex0 : LIGHTMAP;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 result = uTexLightmap.Sample(uTexLightmapSampler, input.tex0) * 3.0;
	return result;
}