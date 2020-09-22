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
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};

// adding constant buffer for transform matrices
cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorld;
	float3 uNoiseParam;
};

#include "../../Noise/HLSL/LibNoise.hlsl"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float3 worldPos = mul(input.pos, uWorld).xyz;
	
	float n = pnoise(worldPos * uNoiseParam.x);
	float weight = clamp(input.tex0.y * uNoiseParam.z, 0.0, 1.0);
	
	float4 noisePosition = input.pos + float4(n * input.norm * weight * uNoiseParam.y, 0.0);
	
	output.pos = mul(noisePosition, uMvpMatrix);
	output.color = input.color;
	output.tex0 = input.tex0;
	
	return output;
}