Texture2DArray uTexIndirect : register(t0);
SamplerState uTexIndirectSampler : register(s0);

Texture2DArray uTexDirectional : register(t1);
SamplerState uTexDirectionalSampler : register(s1);

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

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;
	
	output.Indirect = uTexIndirect.Sample(uTexIndirectSampler, input.tex0) * 3.0;
	output.Directional = uTexDirectional.Sample(uTexDirectionalSampler, input.tex0) * 3.0;
	
	return output;
}