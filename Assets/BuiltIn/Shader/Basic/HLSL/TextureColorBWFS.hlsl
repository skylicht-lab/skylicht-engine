Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 result = input.color * uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	float lum = 0.21 * result.r + 0.72 * result.g + 0.07 * result.b;
	return float4(lum, lum, lum, result.a);
}