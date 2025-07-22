const float PI = 3.1415926;

#include "LibSolverMetallic.glsl"
#if defined(ENABLE_SSR)
#include "../../../SSR/GLSL/LibSSR.glsl"
#endif
#include "../../../PostProcessing/GLSL/LibToneMapping.glsl"

vec3 SG(
	const vec3 baseColor,
	const float spec,
	const float gloss,
	const vec4 position,
	const vec3 worldViewDir,
	const vec3 worldLightDir,
	const vec3 worldNormal,
	const vec3 lightColor,
	const float visibility,
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
	vec3 directionLightColor = sRGB(lightColor);
	vec3 pointLightColor = sRGB(light.rgb);
	vec3 indirectColor = sRGB(indirect.rgb);

	// Lighting
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);

	// Specular
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;

	vec3 envSpecColor = mix(indirectColor * 0.2, vec3(1.0, 1.0, 1.0), visibility);
	vec3 directionalLight = NdotL * directionLightColor * visibility;
	
	// Direction lighting
	vec3 color = (directionalLight * directMultiplier) * diffuseColor * (0.1 + roughness * 0.3);
	color += pointLightColor * lightMultiplier * diffuseColor * 0.5;

	// Direction specular
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
