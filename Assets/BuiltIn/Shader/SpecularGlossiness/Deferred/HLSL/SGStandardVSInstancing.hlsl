// File Generated by Assets/BuildShader.py - source: [SGStandardVS.hlsl : INSTANCING]
struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float4 uSpecGloss: TEXCOORD3;
	float4x4 worldMatrix: TEXCOORD4;
};
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float4 color: COLOR;
	float2 specGloss: SPECGLOSS;
};
cbuffer cbPerObject
{
	float4x4 uVPMatrix;
	float4x4 uView;
};
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.color = input.uColor;
	output.specGloss = input.uSpecGloss.xy;
	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4 uUVScale = input.uvScale;
	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;
	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	output.pos = mul(worldPos, uVPMatrix);
	float4 sampleFragPos = mul(worldPos, uView);
	float sampleDepth = sampleFragPos.z;
	output.worldPosition = float4(worldPos.xyz, sampleDepth);
	output.worldNormal = normalize(worldNormal.xyz);
	return output;
}
