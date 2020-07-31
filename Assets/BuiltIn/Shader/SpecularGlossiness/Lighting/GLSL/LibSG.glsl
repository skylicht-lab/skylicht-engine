const float PI = 3.1415926;

#include "LibSolverMetallic.glsl"

vec3 SG(
	const vec3 baseColor, 
	const float spec, 
	const float gloss,
	const vec3 worldViewDir,
	const vec3 worldLightDir,
	const vec3 worldNormal,
	const vec3 lightColor,
	const float visibility,
	const vec4 light,
	const vec3 indirect,
	const float directMultiplier,
	const float indirectMultiplier)
{
	// Roughness
	float roughness = 1.0 - gloss;
	
	// Metallic
	vec3 f0 = vec3(spec, spec, spec);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	
	// Color
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = baseColor.rgb;
	specularColor = mix(f0, baseColor.rgb, metallic);
	
	// Lighting
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);
	
	// Specular
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	
	vec3 directionalLight = NdotL * lightColor * visibility * directMultiplier;
	vec3 color = (directionalLight + light.rgb) * diffuseColor + specular * specularColor * visibility + light.a * specularColor;
	
	// IBL Ambient
	color += indirect * diffuseColor * indirectMultiplier / PI;
	
	// IBL reflection (fake by ambient)
	// color += indirect * specularColor * metallic;
	
	return color;
}
