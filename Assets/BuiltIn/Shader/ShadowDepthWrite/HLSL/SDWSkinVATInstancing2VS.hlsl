Texture2D uVertexPositionTexture : register(t4);
SamplerState uVertexPositionTextureSampler : register(s4);

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

	// ANIMATION 2 (frame + clipOffset)
	uv.y = (input.uBoneLocation.z + input.uBoneLocation.w + 0.5) / uTextureSize.y;
	float4 skinPosition2 = uVertexPositionTexture.SampleLevel(uVertexPositionTextureSampler, uv, 0.0);
	
	// blend animations
	float4 skinPosition = lerp(skinPosition1, skinPosition2, input.uBlendAnimation.x);

	output.worldPos = mul(float4(skinPosition.xyz, 1.0), uWorldMatrix);
	output.pos = mul(output.worldPos, uVPMatrix);
	output.vPos = output.pos;
	
	return output;
}