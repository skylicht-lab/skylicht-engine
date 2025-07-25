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
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 pos;
	pos = mul(input.pos, uMvpMatrix);
	output.pos = pos;
	return output;
}