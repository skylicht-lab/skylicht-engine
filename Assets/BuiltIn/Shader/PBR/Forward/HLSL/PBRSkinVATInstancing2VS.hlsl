Texture2D uVertexPositionTexture : register(t6);
SamplerState uVertexPositionTextureSampler : register(s6);

Texture2D uVertexNormalTexture : register(t7);
SamplerState uVertexNormalTextureSampler : register(s7);

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
	
	float4 uBoneLocation: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float2 uBlendAnimation: TEXCOORD3;

	float4x4 worldMatrix: TEXCOORD4;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};

cbuffer cbPerObject
{
	float4x4 uVpMatrix;
	float4 uCameraPosition;
	float4 uUVScale;
	float2 uTextureSize;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float2 uv;

	// Vertex id
	uv.x = (input.data.y + 0.5) / uTextureSize.x;

	// ANIMATION 1 (frame + clipOffset)
	uv.y = (input.uBoneLocation.x + input.uBoneLocation.y + 0.5) / uTextureSize.y;
	float4 skinPosition1 = uVertexPositionTexture.SampleLevel(uVertexPositionTextureSampler, uv, 0.0);
	float4 skinNormal1 = uVertexNormalTexture.SampleLevel(uVertexNormalTextureSampler, uv, 0.0);

	// ANIMATION 2 (frame + clipOffset)
	uv.y = (input.uBoneLocation.z + input.uBoneLocation.w + 0.5) / uTextureSize.y;
	float4 skinPosition2 = uVertexPositionTexture.SampleLevel(uVertexPositionTextureSampler, uv, 0.0);
	float4 skinNormal2 = uVertexNormalTexture.SampleLevel(uVertexNormalTextureSampler, uv, 0.0);
	
	// blend animations
	float4 skinPosition = lerp(skinPosition1, skinPosition2, input.uBlendAnimation.x);
	float4 skinNormal = lerp(skinNormal1, skinNormal2, input.uBlendAnimation.x);

	float4 worldPos = mul(float4(skinPosition.xyz, 1.0), uWorldMatrix);
	float4 worldNormal = mul(float4(skinNormal.xyz, 0.0), uWorldMatrix);
	
	float4 worldViewDir = normalize(uCameraPosition - worldPos);
	
	output.worldPosition = worldPos.xyz;
	output.worldNormal = normalize(worldNormal.xyz);
	output.worldViewDir = worldViewDir.xyz;	

	output.pos = mul(worldPos, uVpMatrix);
	output.viewPosition = output.pos;
	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

	return output;
}