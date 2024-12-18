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

uniform samplerCube uTexReflect;

uniform vec4 uLightColor;
uniform vec4 uColor;
#if defined(NO_TEXTURE) || defined(NO_SPECGLOSS)
uniform vec2 uSpecGloss;
#endif
uniform vec2 uLightMul;
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

out vec4 FragColor;

#include "../../../PostProcessing/GLSL/LibToneMapping.glsl"
#include "../../../SHAmbient/GLSL/SHAmbient.glsl"

const float PI = 3.1415926;
const float MinReflectance = 0.04;

float getPerceivedBrightness(vec3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}


float solveMetallic(vec3 diffuse, vec3 specular, float oneMinusSpecularStrength)
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);

	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;

	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

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

	// Solver metallic
	float spec = specMap.r;
	float gloss = specMap.g;
	float roughness = 1.0 - gloss;

	vec3 diffuseColor = diffuseMap.rgb;

	vec3 f0 = vec3(spec, spec, spec);
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(diffuseColor, f0, oneMinusSpecularStrength);

	f0 = vec3(0.1, 0.1, 0.1);	
	vec3 specularColor = mix(f0, diffuseColor, metallic);

	// SH Ambient
	vec3 ambientLighting = shAmbient(n);
	
	// Tone Mapping
	ambientLighting = sRGB(ambientLighting);
	diffuseColor = sRGB(diffuseColor);
	specularColor = sRGB(specularColor);
	vec3 lightColor = sRGB(uLightColor.rgb);

	float c = (1.0 - spec * gloss);

	// Lighting
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
	vec3 color = (directionalLight * diffuseColor) * (0.1 + roughness * 0.3) * c * uLightMul.y;

	// Specular
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;

	// IBL lighting
	color += ambientLighting * diffuseColor * (0.1 + c * 0.9) / PI;

	// IBL reflection
	vec3 reflection = -normalize(reflect(vWorldViewDir, n));
	
	float brightness = (0.8 + gloss * 1.8);
	color += sRGB(textureLod(uTexReflect, reflection, roughness * 7.0).xyz) * brightness * specularColor;

	FragColor = vec4(color, diffuseMap.a);
}