#ifndef NO_TEXTURE
Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);

#ifndef NO_NORMAL_MAP
Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);
#endif

#ifndef NO_SPECGLOSS
Texture2D uTexSpecularMap : register(t2);
SamplerState uTexSpecularMapSampler : register(s2);
#endif
#endif

#if defined(SHADOW)
Texture2DArray uShadowMap : register(t6);
SamplerState uShadowMapSampler : register(s6);
#endif

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float4 viewPosition: VIEWPOSITION;
#ifdef SHADOW
	float3 depth: DEPTH;
	float4 shadowCoord: SHADOWCOORD;
#endif
};
#else
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
#ifdef SHADOW
	float3 depth: DEPTH;
	float4 shadowCoord: SHADOWCOORD;
#endif	
};
#endif

cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uColor;
#if defined(NO_TEXTURE) || defined(NO_SPECGLOSS)
	float2 uSpecGloss;
#endif
	float2 uLightMul;
	float4 uSHConst[4];
};

#include "../../PostProcessing/HLSL/LibToneMapping.hlsl"
#include "../../SHAmbient/HLSL/SHAmbient.hlsl"

#ifdef SHADOW
#include "../../Shadow/HLSL/LibShadow.hlsl"
#endif

static const float PI = 3.1415926;

float4 main(PS_INPUT input) : SV_TARGET
{
#ifdef NO_TEXTURE
	float4 diffuseMap = uColor;
	float3 specMap = float3(uSpecGloss, 0.0);
#else
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0) * uColor;
	#ifdef NO_SPECGLOSS
	float3 specMap = float3(uSpecGloss, 0.0);
	#else
	float3 specMap = uTexSpecularMap.Sample(uTexSpecularMapSampler, input.tex0).xyz;
	#endif
#endif

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
	float3 n = input.worldNormal;
#else
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
	n = normalize(n);
#endif

#if defined(SHADOW)
	// shadow
	float depth = length(input.depth);
	float visibility = shadow(input.shadowCoord, depth);
#endif

	// SH4 Ambient
	float3 ambientLighting = shAmbient(n);

	// Tone Mapping
	ambientLighting = sRGB(ambientLighting);
	float3 diffuseColor = sRGB(diffuseMap.rgb);	
	float3 lightColor = sRGB(uLightColor.rgb);

	float spec = specMap.r;
	float gloss = specMap.g;

	// Lighting
	float NdotL = max(dot(n, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * lightColor;
	float3 color = directionalLight * diffuseColor * 0.3 * uLightMul.y;

	// Specular
	float3 f0 = float3(0.1, 0.1, 0.1);
	float3 specularColor = lerp(f0, diffuseColor, 1.0 - gloss);
	
	float3 H = normalize(input.worldLightDir + input.worldViewDir);
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;

#if defined(SHADOW)
	color *= visibility;
#endif

	// IBL lighting
	color += ambientLighting * diffuseColor / PI;
	
	return float4(color, diffuseMap.a);
}