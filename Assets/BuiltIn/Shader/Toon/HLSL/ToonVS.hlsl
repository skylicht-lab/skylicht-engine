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
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldPos: WORLDPOSITION;
	float3 depth: DEPTH;
	float4 color: COLOR;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
	float4 uUVScale;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldNormal = mul(float4(input.norm.xyz, 0.0), uWorldMatrix);
	
	output.worldPos = worldPos.xyz;
	output.depth = uCameraPosition.xyz - worldPos.xyz;
	output.worldViewDir = normalize(output.depth);
	
	output.pos = mul(input.pos, uMvpMatrix);
	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;
	output.worldNormal = normalize(worldNormal.xyz);

	output.color = input.color;

	return output;
}