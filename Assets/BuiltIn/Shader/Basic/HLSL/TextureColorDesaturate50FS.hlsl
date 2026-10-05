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
	float4 color = input.color * uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	float lum = 0.21 * color.r + 0.72 * color.g + 0.07 * color.b;
	float3 desaturated = lerp(float3(lum, lum, lum), color.rgb, 0.7);
	return float4(desaturated, color.a * 0.75);
}
