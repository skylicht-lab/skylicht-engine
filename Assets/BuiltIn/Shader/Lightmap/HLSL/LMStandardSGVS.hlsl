struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	
	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float2 uSpecGloss: TEXCOORD3;
	float3 sh0: TEXCOORD4;
	float3 sh1: TEXCOORD5;
	float3 sh2: TEXCOORD6;
	float3 sh3: TEXCOORD7;
	float4x4 worldMatrix: TEXCOORD8;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 sh0: SH1;
	float3 sh1: SH2;
	float3 sh2: SH3;
	float3 sh3: SH4;
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

	output.sh0 = input.sh0;
	output.sh1 = input.sh1;
	output.sh2 = input.sh2;
	output.sh3 = input.sh3;

	return output;
}