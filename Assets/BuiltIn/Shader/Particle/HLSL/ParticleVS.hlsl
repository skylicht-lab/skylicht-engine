struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	
	float3 particlePos: TEXCOORD1;
	float4 particleColor: TEXCOORD2;
	float2 particleUVScale: TEXCOORD3;
	float2 particleUVOffset: TEXCOORD4;
	float2 particleSizeRotation: TEXCOORD5;
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
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float3 position = input.pos.xyz * input.particleSizeRotation.x + input.particlePos;
	
	output.pos = mul(float4(position, 1.0), uMvpMatrix);
	output.color = input.particleColor;
	output.tex0 = input.tex0 * input.particleUVScale + input.particleUVOffset;
	
	return output;
}