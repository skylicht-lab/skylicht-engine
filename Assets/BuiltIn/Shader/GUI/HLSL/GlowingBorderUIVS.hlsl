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
	float2 tex1 : TEXCOORD1;
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4 uUVScale;
	float4 uTime;
	float2 uData; // Speed
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, uMvpMatrix);
	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;
	output.tex1 = float2(input.norm.x, input.norm.y);
	
	float angle = uTime.x * uData.x / 6.0f;
	float pi2 = 3.14159265359f * 2.0f;
	float cosA = cos(angle * pi2);
	float sinA = sin(angle * pi2);

	float2x2 rotationMatrix = float2x2(
		cosA, -sinA,
		sinA, cosA
	);

	float2 center = float2(0.5f, 0.5f);
	float2 uv = output.tex1 - center;

	float2 rotatedUV = mul(uv, rotationMatrix); // Hoặc: rotationMatrix * uv;
	rotatedUV *= 0.7;
	rotatedUV += center;
	
	output.tex1 = rotatedUV;
	
	output.color = input.color;
	return output;
}