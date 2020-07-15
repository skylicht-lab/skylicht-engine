struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float2 tex1: TEXCOORD1;
	float3 lightmap: LIGHTMAP;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 tex0 : LIGHTMAP;
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
	output.tex0 = input.lightmap;
	
	return output;
}