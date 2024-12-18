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
	float roughness = 1.0 - gloss;

	vec3 diffuseColor = baseColor.rgb;

	// Metallic
	vec3 f0 = vec3(spec, spec, spec);
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, f0, oneMinusSpecularStrength);

	// Color
	f0 = vec3(0.1, 0.1, 0.1);
	vec3 specularColor = mix(f0, baseColor.rgb, metallic);

	// Tone mapping
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	vec3 directionColor = sRGB(light.rgb);
	vec3 indirectColor = sRGB(indirect.rgb);

	// Specular
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	
	// Direction lighting
	vec3 color = (directionColor * lightMultiplier) * diffuseColor;
	
	// Direction specular
	float visibility = light.a;
	vec3 envSpecColor = mix(indirectColor, vec3(1.0, 1.0, 1.0), visibility);
	color += specular * specularColor * envSpecColor;

	// IBL Ambient
	color += indirectColor * diffuseColor * indirectMultiplier / PI;

	// IBL reflection
#if defined(ENABLE_SSR)
	float brightness = (0.8 + gloss * 1.8);
	vec3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * brightness * specularColor;
#endif

	return color;
}
