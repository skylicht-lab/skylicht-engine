Texture2DArray uTexLightmap : register(t0);
SamplerState uTexLightmapSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 tex0 : LIGHTMAP;
};

struct PS_OUTPUT
{
	float4 Indirect: SV_TARGET0;
	float4 Directional: SV_TARGET1;
};

cbuffer cbPerFrame
{
	float uLightmapIndex;
}

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;
	
	output.Indirect = uTexLightmap.Sample(uTexLightmapSampler, input.tex0) * 3.0;
	
	float3 uv = input.tex0;
	uv.z += uLightmapIndex;	
	output.Directional = uTexLightmap.Sample(uTexLightmapSampler, uv) * 3.0;
	
	return output;
}