struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#if defined(LM)
	float2 tex1: TEXCOORD1;
	float3 lightmap: LIGHTMAP;
#endif
#if defined(NORMAL_MAP)
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 data : DATA;
#endif
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldPosition: WORLDPOSITION;
	float2 tex0: TEXCOORD0;
#if defined(NORMAL_MAP)	
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
#endif
};

cbuffer cbPerObject
{
	float4x4 uWorldMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
#if defined(LM)	
	float2 lightmapUV = input.lightmap.xy;
#else
	float2 lightmapUV = input.tex0;
#endif

	output.tex0 = input.tex0;

	// convert [0 - 1] to [-1 - 1]
	lightmapUV.y = 1.0 - lightmapUV.y;
	lightmapUV = lightmapUV * 2.0 - float2(1.0, 1.0);	
	
	output.pos = float4(lightmapUV, 0.0, 1.0);

	// world
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	output.worldNormal = normalize(worldNormal.xyz);
	
#if defined(NORMAL_MAP)
	float4 worldTangent = mul(float4(input.tangent.xyz, 0.0), uWorldMatrix);
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
	output.tangentw = input.data.x;
#endif	
	
	output.worldPosition = mul(input.pos, uWorldMatrix).xyz;
	
	return output;
}