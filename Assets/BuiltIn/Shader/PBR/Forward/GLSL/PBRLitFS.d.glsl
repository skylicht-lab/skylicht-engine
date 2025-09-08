#if defined(SHADOW)
precision highp float;
#else
precision mediump float;
#endif

#ifndef NO_TEXTURE
uniform sampler2D uTexAlbedo;
#ifndef NO_NORMAL_MAP
uniform sampler2D uTexNormal;
#endif
uniform sampler2D uTexRMA;
#endif
#ifdef LM
uniform sampler2DArray uTexLightmap;
#endif

#if defined(PLANAR_REFLECTION)
uniform sampler2D uTexReflect;
#else
uniform samplerCube uTexReflect;
#endif

#if defined(SHADOW)
uniform sampler2DArray uShadowMap;
#endif

uniform sampler2D uTexBRDF;

#ifdef EMISSIVE
uniform sampler2D uTexEmissive;
#endif

uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec4 uColor;
#ifdef EMISSIVE
uniform vec4 uEmissive;
#endif
#ifdef NO_TEXTURE
uniform vec2 uRoughnessMetal;
#endif
uniform vec4 uSHConst[4];

in vec2 vTexCoord0;
#ifdef LM
in vec3 vTexCoord1;
#endif
in vec3 vWorldNormal;
in vec3 vWorldViewDir;

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
#endif

in vec4 vViewPosition;
in vec3 vWorldPosition;

#ifdef SHADOW
in vec3 vDepth;
in vec4 vShadowCoord;
#endif

#if defined(PLANAR_REFLECTION)
in vec4 vReflectCoord;
#endif

out vec4 FragColor;

#include "../../../PostProcessing/GLSL/LibToneMapping.glsl"
#include "../../../SHAmbient/GLSL/SHAmbient.glsl"

#ifdef SHADOW
#include "../../../Shadow/GLSL/LibShadow.glsl"
#endif

const float PI = 3.1415926;

// Code from:
// https://github.com/ddrevicky/physically-based-shading/blob/master/src/shaders/PBR.frag
// https://github.com/Nadrin/PBR/blob/master/data/shaders/glsl/pbr_fs.glsl

vec3 fresnelSchlick(vec3 V, vec3 H, vec3 F0)
{
	float VdotH = max(dot(V, H), 0.001);
	return F0 + (1.0 - F0) * pow((1.0 - VdotH), 5.0);
}

float gaSchlickG1(vec3 N, vec3 V, float k)
{
	float NdotV = max(dot(N, V), 0.001);
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k; 
	return nom / denom;
}

float schlickGGX(vec3 N, vec3 V, vec3 L, float roughness)
{
	float k = 0.001 + (roughness + 1.0) * (roughness + 1.0) / 8.0;
	return gaSchlickG1(N, L, k) * gaSchlickG1(N, V, k);
}

float ndfGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);

	float nom = a2;
	float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;
	return nom / denom;
}

#ifdef LM
vec3 computeLightContribution(vec3 N, vec3 L, vec3 V, vec3 F0, vec3 lambert, vec3 lightmapColor, float VdotN, float roughness, float metalness)
{
	vec3 H = normalize(L + V);
	
	// Cook-Torrance Specular
	float D = ndfGGX(N, H, roughness);
	float G = schlickGGX(N, V, L, roughness);
	vec3 F = fresnelSchlick(V, H, F0);
	
	vec3 specularBRDF = (D * F * G) / (4.0 * VdotN + 0.001);
	
	vec3 kd = mix(vec3(1.0, 1.0, 1.0) - F, vec3(0.0, 0.0, 0.0), metalness);
	
	vec3 BRDF = kd * lambert + specularBRDF;
	return BRDF * lightmapColor;
}
#else
vec3 computeLightContribution(vec3 N, vec3 L, vec3 V, vec3 F0, vec3 lambert, vec3 lightColor, float VdotN, float roughness, float metalness)
{
	vec3 H = normalize(L + V);
	
	// Cook-Torrance Specular
	float D = ndfGGX(N, H, roughness);
	float G = schlickGGX(N, V, L, roughness);
	vec3 F = fresnelSchlick(V, H, F0);
	
	float LdotN = max(dot(N, L), 0.0);
	
	vec3 specularBRDF = (D * F * G) / (4.0 * VdotN * LdotN + 0.001);
	
	vec3 kd = mix(vec3(1.0, 1.0, 1.0) - F, vec3(0.0, 0.0, 0.0), metalness);
	
	vec3 BRDF = kd * lambert + specularBRDF;
	return BRDF * lightColor * LdotN;
}
#endif

void main(void)
{
#ifdef NO_TEXTURE
	vec4 albedoMap = uColor;
	vec3 rmaMap = vec3(uRoughnessMetal, 1.0);
#else
	vec4 albedoMap = texture(uTexAlbedo, vTexCoord0.xy) * uColor;
	vec3 rmaMap = texture(uTexRMA, vTexCoord0.xy).xyz;
#endif

#ifdef EMISSIVE
	vec3 emissiveMap = texture(uTexEmissive, vTexCoord0.xy).rgb * uEmissive.rgb * uEmissive.a;
#endif

#ifdef LM
	vec4 lightmap = texture(uTexLightmap, vTexCoord1);
#endif

#if defined(NO_NORMAL_MAP) || defined(NO_TEXTURE)
	vec3 n = vWorldNormal;
#else
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;
	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y *= vTangentW;
	vec3 n = normalize(rotation * localCoords);
	n = normalize(n);
#endif

#if defined(SHADOW)
	// shadow
	float depth = length(vDepth);
	float visibility = shadow(vShadowCoord, depth);
#ifdef LM
	visibility = min(visibility, lightmap.a);
#endif
#endif

	// Solver metallic
	float roughness = rmaMap.r;
	float metalness = rmaMap.g;
	float ao = rmaMap.b;

	// SH Ambient
	vec3 ambientLighting = shAmbient(n);

	// Tone Mapping
	ambientLighting = sRGB(ambientLighting);
	
	vec3 albedo = sRGB(albedoMap.rgb);
	
	// Dielectric
	vec3 F0 = vec3(0.04, 0.04, 0.04);
	F0 = mix(F0, albedo, metalness);
	
	// VdotN
	float VdotN = max(dot(vWorldViewDir, n), 0.0);
	
	// Lambert
	vec3 lambert = albedo / PI;
	
	// Lighting
#ifdef LM
	vec3 lightContribution = computeLightContribution(n, uLightDirection.xyz, vWorldViewDir, F0, lambert, sRGB(lightmap.rgb * 3.0), VdotN, roughness, metalness);
#else	
	vec3 lightContribution = computeLightContribution(n, uLightDirection.xyz, vWorldViewDir, F0, lambert, sRGB(uLightColor.rgb), VdotN, roughness, metalness);
#endif

#if defined(SHADOW)
	lightContribution *= visibility;
#endif

	// IBL diffuse
	vec3 F = fresnelSchlick(vWorldViewDir, n, F0);
	vec3 kd = vec3(1.0) - F;
	kd *= (1.0 - metalness);

	vec3 indirectDiffuse = ambientLighting * lambert;

#if defined(PLANAR_REFLECTION)
	// projection uv
	vec3 reflectUV = vReflectCoord.xyz / vReflectCoord.w;
	#if defined(REFLECTION_MIPMAP)
	vec3 prefilteredColor = textureLod(uTexReflect, reflectUV.xy, roughness * 7.0).xyz;
	#else
	vec3 prefilteredColor = textureLod(uTexReflect, reflectUV.xy, 0.0).xyz;
	#endif
#else
	// IBL reflection
	vec3 reflection = -normalize(reflect(vWorldViewDir, n));	
	// reflection should be the size: 128x128
	vec3 prefilteredColor = sRGB(textureLod(uTexReflect, reflection, roughness * 7.0).xyz);
#endif

	// Get F scale and bias from the LUT
	vec2 envBRDF = texture(uTexBRDF, vec2(VdotN, roughness)).rg;
	vec3 indirectSpecular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);

	float brightness = (0.8 + (1.0 - roughness) * 2.2);
	vec3 indirectLight = (kd * indirectDiffuse + indirectSpecular * brightness);

#ifdef EMISSIVE
	lightContribution += sRGB(emissiveMap);
#endif

	FragColor = vec4((lightContribution + indirectLight) * ao, albedoMap.a);
}