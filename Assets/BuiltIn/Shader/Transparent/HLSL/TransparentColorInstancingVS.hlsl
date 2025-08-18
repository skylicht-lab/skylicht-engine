struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float4 uUVScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float4x4 worldMatrix: TEXCOORD3;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerObject
{
	float4x4 uVPMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4 worldPos = mul(input.pos, uWorldMatrix);
	output.pos = mul(worldPos, uVPMatrix);
	
	output.color = input.uColor;
	output.tex0 = input.tex0 * input.uvScale.xy + input.uvScale.zw;

	return output;
}