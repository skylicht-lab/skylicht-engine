Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
#ifdef TANGENT
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
#endif
#ifdef INSTANCING
	float4 color: COLOR;
	float2 specGloss: SPECGLOSS;
#endif	
};

struct PS_OUTPUT
{
	float4 Diffuse: SV_TARGET0;
	float4 Position: SV_TARGET1;
	float4 Normal: SV_TARGET2;
	float4 SG: SV_TARGET3;
};

#ifndef INSTANCING
cbuffer cbPerFrame
{
	float4 uColor;
	float2 uSpecGloss;
};
#endif

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;

	float3 baseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0).rgb;

#ifdef INSTANCING
	output.Diffuse = float4(baseMap * input.color.rgb, 1.0);
#else
	output.Diffuse = float4(baseMap * uColor.rgb, 1.0);
#endif

	output.Position = input.worldPosition;
	output.Normal = float4(input.worldNormal, 1.0);
	
#ifdef INSTANCING
	output.SG = float4(input.specGloss.x, input.specGloss.y, 0.0, 1.0);
#else
	output.SG = float4(uSpecGloss.x, uSpecGloss.y, 0.0, 1.0);
#endif

	return output;
}
