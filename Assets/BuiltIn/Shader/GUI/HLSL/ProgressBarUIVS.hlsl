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
	float4 uUVTiling; // TileX;TileY;Speed
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, uMvpMatrix);
	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;
	output.tex1 = float2(input.norm.x, input.norm.y);
	output.tex1 = output.tex1 * uUVTiling.xy + uUVTiling.zw + float2(uTime.x * uUVTiling.z, 0.0);
	output.color = input.color;
	return output;
}