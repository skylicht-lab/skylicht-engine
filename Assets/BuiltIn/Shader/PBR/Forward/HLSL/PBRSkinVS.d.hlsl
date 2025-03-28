struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 data : DATA;
	float4 blendIndex : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHT;
};

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};
#else
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};
#endif

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
	float4 uUVScale;
	float4x4 uBoneMatrix[64];
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 skinMatrix;
	float4 skinPosition;
	float4 skinNormal;
	float4 skinTangent;

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
	skinNormal = mul(float4(input.norm, 0.0), skinMatrix);
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)	
	skinTangent = mul(float4(input.tangent, 0.0), skinMatrix);
#endif

	output.pos = mul(skinPosition, uMvpMatrix);

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)	
	output.tangentw = input.data.x;
#endif

	float4 worldPos = mul(skinPosition, uWorldMatrix);
	float4 worldViewDir = normalize(uCameraPosition - worldPos);

	float4 worldNormal = mul(float4(skinNormal.xyz, 0.0), uWorldMatrix);
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)	
	float4 worldTangent = mul(float4(skinTangent.xyz, 0.0), uWorldMatrix);
#endif
	output.worldPosition = worldPos.xyz;

	output.worldNormal = normalize(worldNormal.xyz);
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)	
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
#endif

	output.worldViewDir = worldViewDir.xyz;
	output.viewPosition = output.pos;

	return output;
}