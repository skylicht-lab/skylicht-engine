struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 data : DATA;
#endif
#ifdef INSTANCING
	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float4 uSpecGloss: TEXCOORD3;
	float4x4 worldMatrix: TEXCOORD4;
#endif
};

#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
#ifdef SHADOW
	float3 depth: DEPTH;
	float4 shadowCoord: SHADOWCOORD;
#endif
};
#else
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
#ifdef SHADOW
	float3 depth: DEPTH;
	float4 shadowCoord: SHADOWCOORD;
#endif
#if defined(PLANAR_REFLECTION)
	float4 reflectCoord: REFLECTIONCOORD;
#endif	
};
#endif

cbuffer cbPerObject
{
#ifdef INSTANCING
	float4x4 uVPMatrix;
#else	
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
#endif

	float4 uCameraPosition;
	float4 uLightDirection;
	
#ifndef INSTANCING	
	float4 uUVScale;
#endif

#ifdef SHADOW
	float4x4 uShadowMatrix;
#endif

#if defined(PLANAR_REFLECTION)
	float4x4 uRTTMatrix;
#endif
};

VS_OUTPUT main(VS_INPUT input)
{
#ifdef INSTANCING
	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4 uUVScale = input.uvScale;
#endif	
	
	VS_OUTPUT output;

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;

#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	output.tangentw = input.data.x;
#endif

	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldViewDir = normalize(uCameraPosition - worldPos);
	float4 worldNormal = mul(float4(input.norm.xyz, 0.0), uWorldMatrix);
	
#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	float4 worldTangent = mul(float4(input.tangent.xyz, 0.0), uWorldMatrix);
#endif

	output.worldNormal = normalize(worldNormal.xyz);
	
#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
#endif

#ifdef SHADOW
	output.depth = uCameraPosition.xyz - worldPos.xyz;
	output.shadowCoord = mul(float4(worldPos.xyz, 1.0), uShadowMatrix);
#endif

#if defined(PLANAR_REFLECTION)
	output.reflectCoord = mul(float4(worldPos.xyz, 1.0), uRTTMatrix);
#endif

	output.worldViewDir = worldViewDir.xyz;
	output.worldLightDir = normalize(uLightDirection.xyz);

#ifdef INSTANCING
	output.pos = mul(worldPos, uVPMatrix);
#else
	output.pos = mul(input.pos, uMvpMatrix);
#endif

	return output;
}