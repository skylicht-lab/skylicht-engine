struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4x4 uView;
	float4 uUVScale;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = mul(input.pos, uMvpMatrix);
	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);

	float4 sampleFragPos = mul(worldPos, uView);
	float sampleDepth = sampleFragPos.z;

	output.worldPosition = float4(worldPos.xyz, sampleDepth);
	output.worldNormal = normalize(worldNormal.xyz);

	return output;
}