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
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

// adding constant buffer for transform matrices
cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uBoneMatrix[64];
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float4x4 skinMatrix;
	float4 skinPosition;
	//float4 skinNormal;
	
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
	//skinNormal 	= mul(float4(input.norm, 0.0), skinMatrix);
	
	output.pos = mul(skinPosition, uMvpMatrix);
	output.color = input.color;
	output.tex0 = input.tex0;
	return output;
}