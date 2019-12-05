struct VS_INPUT
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 color	: COLOR;
	float2 tex0		: TEXCOORD0;
	float2 tex1		: TEXCOORD1;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
};

// adding constant buffer for transform matrices
cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.pos = mul(input.pos, uMvpMatrix);
	output.color = input.color;
	output.tex0 = input.tex0;
	output.tex1 = input.tex1;
	
	return output;
}