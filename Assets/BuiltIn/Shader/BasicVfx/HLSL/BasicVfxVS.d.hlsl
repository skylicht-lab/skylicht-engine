struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#ifdef UV2
	float2 tex1: TEXCOORD1;
#endif
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
#ifdef UV2
	float2 tex1 : TEXCOORD1;
#endif
	float4 uvScale: UVSCALE;
#ifdef RIM_LIGHT
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
#endif
};

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4 uUVScale;
#ifdef RIM_LIGHT
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
#endif
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, uMvpMatrix);
	output.color = input.color;
	output.tex0 = input.tex0;
	output.uvScale = uUVScale;
	
#ifdef UV2	
	output.tex1 = input.tex1;
#endif

#ifdef RIM_LIGHT
	float4 worldPos = mul(input.pos, uWorldMatrix);
	output.worldViewDir = normalize((uCameraPosition - worldPos).xyz);
	
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	output.worldNormal = normalize(worldNormal).xyz;
#endif

	return output;
}