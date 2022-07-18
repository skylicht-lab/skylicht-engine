struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	
	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float2 uSpecGloss: TEXCOORD3;

	float4x4 worldMatrix: TEXCOORD4;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos: POSITION;
};

cbuffer cbPerObject
{
	float4x4 uVPMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4x4 uMvpMatrix = mul(uWorldMatrix, uVPMatrix);

	output.pos = mul(input.pos, uMvpMatrix);
	output.worldPos = mul(input.pos, uWorldMatrix);

	return output;
}