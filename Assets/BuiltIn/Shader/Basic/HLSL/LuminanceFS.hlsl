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
	float lum = max(dot(color.rgb, float3(0.299, 0.587, 0.114)), 0.0001);
	return float4(lum, 0.0, 0.0, 0.0);
}