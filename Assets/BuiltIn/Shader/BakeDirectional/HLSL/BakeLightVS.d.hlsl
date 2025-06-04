struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#if defined(UV2)
	float2 tex1: TEXCOORD1;
	float3 lightmap: LIGHTMAP;
#endif
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldPosition: WORLDPOSITION;
};

cbuffer cbPerObject
{
	float4x4 uWorldMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
#if defined(UV2)	
	float2 lightmapUV = input.lightmap.xy;
#else
	float2 lightmapUV = input.tex0;
#endif

	// convert [0 - 1] to [-1 - 1]
	lightmapUV.y = 1.0 - lightmapUV.y;
	lightmapUV = lightmapUV * 2.0 - float2(1.0, 1.0);	
	
	output.pos = float4(lightmapUV, 0.0, 1.0);

	// world
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	output.worldNormal = normalize(worldNormal.xyz);
	output.worldPosition = mul(input.pos, uWorldMatrix).xyz;
	
	return output;
}