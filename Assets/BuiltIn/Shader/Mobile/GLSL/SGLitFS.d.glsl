precision mediump float;

#ifndef NO_TEXTURE
uniform sampler2D uTexDiffuse;

#ifndef NO_NORMAL_MAP
uniform sampler2D uTexNormal;
#endif

#ifndef NO_SPECGLOSS
uniform sampler2D uTexSpecular;
#endif
#endif

uniform vec4 uLightColor;
uniform vec4 uColor;
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

in vec4 vViewPosition;
in vec3 vWorldPosition;

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"
#include "../../SHAmbient/GLSL/SHAmbient.glsl"

const float PI = 3.1415926;

void main(void)
{
#ifdef NO_TEXTURE
	vec4 diffuseMap = uColor;
	vec3 specMap = vec3(uSpecGloss, 0.0);
#else
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy) * uColor;

	#ifdef NO_SPECGLOSS
	vec3 specMap = vec3(uSpecGloss, 0.0);
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
	
	// SH Ambient
	vec3 ambientLighting = shAmbient(n);
	
	// Tone Mapping
	ambientLighting = sRGB(ambientLighting);
	vec3 diffuseColor = sRGB(diffuseMap.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);

	// Lighting
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
	vec3 color = directionalLight * diffuseColor;

	// Specular
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 100.0f * specMap.g) * specMap.r;
	color += specular * diffuseColor;

	// IBL lighting
	color += ambientLighting * diffuseColor / PI;

	FragColor = vec4(color, diffuseMap.a);
}