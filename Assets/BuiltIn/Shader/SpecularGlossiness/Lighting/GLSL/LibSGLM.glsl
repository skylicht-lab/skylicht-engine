const float PI = 3.1415926;

#include "LibSolverMetallic.glsl"
#if defined(ENABLE_SSR)
#include "../../../SSR/GLSL/LibSSR.glsl"
#endif
#include "../../../PostProcessing/GLSL/LibToneMapping.glsl"

vec3 SGLM(
	const vec3 baseColor,
	const float spec,
	const float gloss,
	const vec4 position,
	const vec3 worldViewDir,
	const vec3 worldLightDir,
	const vec3 worldNormal,
	const vec3 lightColor,
	const vec4 light,
	const vec3 indirect,
	const float directMultiplier,
	const float indirectMultiplier,
	const float lightMultiplier)
{
	// Roughness
	float glossiness = max(gloss, 0.01);
	float roughness = 1.0 - glossiness;

	// Metallic
	vec3 f0 = vec3(spec, spec, spec);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);

	// Color
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = baseColor.rgb;
	specularColor = mix(f0, baseColor.rgb, metallic);

	// Tone mapping
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	vec3 directionColor = sRGB(light.rgb);
	vec3 indirectColor = sRGB(indirect.rgb);

	// Specular
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * glossiness) * spec;

	vec3 envSpecColor = vec3(1.0, 1.0, 1.0);
	
	// Direction lighting
	vec3 color = (directionColor * lightMultiplier) * diffuseColor;
	
	// Direction specular
	color += specular * specularColor * envSpecColor;

	// IBL Ambient
	color += indirectColor * diffuseColor * indirectMultiplier / PI;

	// IBL reflection
#if defined(ENABLE_SSR)	
	vec3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * metallic * specularColor;
#endif

	return color;
}
