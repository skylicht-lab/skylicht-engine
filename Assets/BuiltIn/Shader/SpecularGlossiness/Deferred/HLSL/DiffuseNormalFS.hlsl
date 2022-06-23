Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

Texture2D uTexNormal : register(t1);
SamplerState uTexNormalSampler : register(s1);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
};

struct PS_OUTPUT
{
	float4 Diffuse: SV_TARGET0;
	float4 Position: SV_TARGET1;
	float4 Normal: SV_TARGET2;
	float4 SG: SV_TARGET3;
};

cbuffer cbPerFrame
{
	float4 uColor;
	float2 uSpecGloss;
};

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;

	float3 baseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0).rgb;
	float3 normalMap = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;

	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);

	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = mul(localCoords, rotation);
	n = normalize(n);

	output.Diffuse = float4(baseMap * uColor.rgb, 1.0);
	output.Position = input.worldPosition;
	output.Normal = float4(n, 1.0);
	output.SG = float4(uSpecGloss.x, uSpecGloss.y, 0.0, 1.0);

	return output;
}
