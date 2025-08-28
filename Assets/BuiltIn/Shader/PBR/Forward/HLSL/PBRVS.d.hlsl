struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#if defined(LM)
	float2 tex1: TEXCOORD1;
	float3 lightmap: LIGHTMAP;
#else
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 data : DATA;
#endif
#endif
};

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
#if defined(LM)
	float3 tex1 : TEXCOORD1;
#endif
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
#if defined(PLANAR_REFLECTION)
	float4 reflectCoord: REFLECTIONCOORD;
#endif	
};
#else
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
#if defined(PLANAR_REFLECTION)
	float4 reflectCoord: REFLECTIONCOORD;
#endif
};
#endif

cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4 uCameraPosition;
	float4 uUVScale;
#if defined(PLANAR_REFLECTION)
	float4x4 uRTTMatrix;
#endif
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, uMvpMatrix);

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;
#if defined(LM)
	output.tex1 = input.lightmap;
#endif

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	output.tangentw = input.data.x;
#endif

	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldViewDir = normalize(uCameraPosition - worldPos);
	float4 worldNormal = mul(float4(input.norm.xyz, 0.0), uWorldMatrix);
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	float4 worldTangent = mul(float4(input.tangent.xyz, 0.0), uWorldMatrix);
#endif

	output.worldPosition = worldPos.xyz;

	output.worldNormal = normalize(worldNormal.xyz);
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)	
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
#endif

#if defined(PLANAR_REFLECTION)
	output.reflectCoord = mul(float4(worldPos.xyz, 1.0), uRTTMatrix);
#endif

	output.worldViewDir = worldViewDir.xyz;
	output.viewPosition = output.pos;

	return output;
}