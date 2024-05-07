Texture2D uTransformTexture : register(t4);
SamplerState uTransformTextureSampler : register(s4);

struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float3 tangent: TANGENT;
	float3 binormal: BINORMAL;
	float2 data: DATA;
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
	float4 vPos: POSITION;
	float4 worldPos: WORLD_POSITION;
};

cbuffer cbPerObject
{
	float4x4 uVPMatrix;
	float4 uAnimation;
	float2 uTransformTextureSize;
};

#include "../../TransformTexture/HLSL/LibTransformTexture.hlsl"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4x4 uMvpMatrix = mul(uWorldMatrix, uVPMatrix);

	float4x4 skinMatrix;
	float4 skinPosition;

	float2 boneLocation = input.uBoneLocation.xy;
	float boneLocationY = input.uBoneLocation.y;

	// bone 0
	boneLocation.y = boneLocationY + input.blendIndex[0];
	skinMatrix = input.blendWeight[0] * getTransformFromTexture(boneLocation);

	// bone 1
	boneLocation.y = boneLocationY + input.blendIndex[1];
	skinMatrix += input.blendWeight[1] * getTransformFromTexture(boneLocation);

	// bone 2
	boneLocation.y = boneLocationY + input.blendIndex[2];
	skinMatrix += input.blendWeight[2] * getTransformFromTexture(boneLocation);

	// bone 3
	boneLocation.y = boneLocationY + input.blendIndex[3];
	skinMatrix += input.blendWeight[3] * getTransformFromTexture(boneLocation);

	// skin result
	skinPosition = mul(input.pos, skinMatrix);

	output.pos = mul(skinPosition, uMvpMatrix);
	output.vPos = output.pos;
	output.worldPos = mul(skinPosition, uWorldMatrix);
	
	return output;
}