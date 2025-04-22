#if defined(SHADOW)
precision highp float;
#else
precision mediump float;
#endif

precision highp sampler2D;
precision highp sampler2DArray;

#ifndef NO_TEXTURE
uniform sampler2D uTexDiffuse;

#ifndef NO_NORMAL_MAP
uniform sampler2D uTexNormal;
#endif

#ifndef NO_SPECGLOSS
uniform sampler2D uTexSpecular;
#endif
#endif

#if defined(SHADOW)
uniform sampler2DArray uShadowMap;
#endif

uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec2 uLightMul;

#if defined(NO_TEXTURE) || defined(NO_SPECGLOSS)
uniform vec2 uSpecGloss;
#endif
uniform vec4 uSHConst[4];

in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
#endif

#ifdef SHADOW
in vec3 vDepth;
in vec4 vShadowCoord;
#endif

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"
#include "../../SHAmbient/GLSL/SHAmbient.glsl"

#ifdef SHADOW
#include "../../Shadow/GLSL/LibShadow.glsl"
#endif

const float PI = 3.1415926;

void main(void)
{
#ifdef NO_TEXTURE
	vec4 diffuseMap = uColor;
	vec3 specMap = vec3(uSpecGloss, 1.0);
#else
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy) * uColor;

	#ifdef NO_SPECGLOSS
	vec3 specMap = vec3(uSpecGloss, 1.0);
	#else
	vec3 specMap = texture(uTexSpecular, vTexCoord0.xy).xyz;
	#endif
	
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
#endif
	
	// SH Ambient
	vec3 ambientLighting = shAmbient(n);
	
	// Tone Mapping
	ambientLighting = sRGB(ambientLighting);
	vec3 diffuseColor = sRGB(diffuseMap.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);

	float spec = specMap.r;
	float gloss = specMap.g;

#if defined(AO)
	float ao = specMap.b;
#endif

	// Lighting
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
#if defined(AO)
	directionalLight *= ao;
#endif		
	vec3 color = directionalLight * diffuseColor * 0.3 * uLightMul.y;

	// Specular	
	vec3 specularColor = vec3(0.5, 0.5, 0.5);
	
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
#if defined(AO)
	specular *= ao;
	ambientLighting *= ao;
#endif
	color += specular * specularColor * uLightMul.x;

#if defined(SHADOW)
	color *= visibility;
#endif

	// IBL lighting
	color += ambientLighting * diffuseColor / PI;

	FragColor = vec4(color, diffuseMap.a);
}