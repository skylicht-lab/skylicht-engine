struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float3 worldNormal: WORLDNORMAL;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);

	output.pos = mul(input.pos, uMvpMatrix);
	output.color = input.color;
	output.worldNormal = normalize(worldNormal.xyz);

	return output;
}