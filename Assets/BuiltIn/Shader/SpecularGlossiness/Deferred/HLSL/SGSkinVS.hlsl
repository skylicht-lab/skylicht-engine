struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float4 blendIndex : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHT;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4x4 uView;
	float4 uUVScale;
	float4x4 uBoneMatrix[64];
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 skinMatrix;
	float4 skinPosition;
	float3 skinNormal;

	// bone 0
	int index = int(input.blendIndex[0]);
	skinMatrix = input.blendWeight[0] * uBoneMatrix[index];

	// bone 1
	index = int(input.blendIndex[1]);
	skinMatrix += input.blendWeight[1] * uBoneMatrix[index];

	// bone 2
	index = int(input.blendIndex[2]);
	skinMatrix += input.blendWeight[2] * uBoneMatrix[index];

	// bone 3
	index = int(input.blendIndex[3]);
	skinMatrix += input.blendWeight[3] * uBoneMatrix[index];

	// skin result
	skinPosition = mul(input.pos, skinMatrix);
	skinNormal = mul(float4(input.norm, 0.0), skinMatrix).xyz;

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

	float4 worldPos = mul(skinPosition, uWorldMatrix);
	float4 worldNormal = mul(float4(skinNormal, 0.0), uWorldMatrix);

	output.pos = mul(skinPosition, uMvpMatrix);

	float4 sampleFragPos = mul(worldPos, uView);
	float sampleDepth = sampleFragPos.z;

	output.worldPosition = float4(worldPos.xyz, sampleDepth);
	output.worldNormal = normalize(worldNormal.xyz);

	return output;
}