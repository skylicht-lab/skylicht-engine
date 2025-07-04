struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float3 tangent: TANGENT;
	float3 binormal: BINORMAL;
	float2 data: DATA;
	float3 D0: TEXCOORD1;
	float3 D1: TEXCOORD2;
	float3 D2: TEXCOORD3;
	float3 D3: TEXCOORD4;
	float4x4 worldMatrix: TEXCOORD5;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 D0: SH1;
	float3 D1: SH2;
	float3 D2: SH3;
	float3 D3: SH4;
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
	output.worldPosition = mul(input.pos, uWorldMatrix);
	
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	output.worldNormal = normalize(worldNormal.xyz);

	output.D0 = input.D0;
	output.D1 = input.D1;
	output.D2 = input.D2;
	output.D3 = input.D3;

	return output;
}