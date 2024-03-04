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
	float3 worldLightDir: WORLDLIGHTDIR;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uUVScale;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, uMvpMatrix);

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldViewDir = normalize(uCameraPosition - worldPos);

	float4 worldNormal = mul(float4(input.norm.xyz, 0.0), uWorldMatrix);

	output.worldPosition = worldPos.xyz;
	output.worldNormal = normalize(worldNormal.xyz);

	output.worldViewDir = worldViewDir.xyz;
	output.worldLightDir = normalize(uLightDirection.xyz);
	output.viewPosition = output.pos;

	return output;
}