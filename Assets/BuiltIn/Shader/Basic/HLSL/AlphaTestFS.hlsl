Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexAlpha : register(t1);
SamplerState uTexAlphaSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 alphaColor = uTexAlpha.Sample(uTexAlphaSampler, input.tex0);
	if (alphaColor.r < 0.5)
		discard;
	float4 result = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	return result;
}