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
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float4 worldPos: WORLD_POSITION;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, uMvpMatrix);
	output.color = input.color;
	output.tex0 = input.tex0;
	output.worldPos = mul(input.pos, uWorldMatrix);
	
	return output;
}