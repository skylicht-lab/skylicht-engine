static const float PI = 3.1415926;

#include "LibSolverMetallic.hlsl"
#if defined(ENABLE_SSR)
#include "../../../SSR/HLSL/LibSSR.hlsl"
#endif
#include "../../../PostProcessing/HLSL/LibToneMapping.hlsl"

float3 SGLM(
	const float3 baseColor,
	const float spec,
	const float gloss,
	const float4 position,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 lightColor,
	const float4 light,
	const float3 indirect,
	const float directMultiplier,
	const float indirectMultiplier,
	const float lightMultiplier)
{
	// Roughness
	float roughness = 1.0 - gloss;

	float3 diffuseColor = baseColor.rgb;

	// Metallic
	float3 f0 = spec;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(diffuseColor, f0, oneMinusSpecularStrength);

	// Color
	f0 = float3(0.1, 0.1, 0.1);
	float3 specularColor = lerp(f0, diffuseColor, metallic);

	// Tone mapping
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	float3 directionColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);

	// Specular
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	
	// Direction lighting
	float3 color = (directionColor * lightMultiplier) * diffuseColor * (0.1 + roughness * 0.3);
	
	// Direction specular
	float visibility = light.a;
	float3 envSpecColor = lerp(indirectColor * 0.2, float3(1.0, 1.0, 1.0), visibility);
	color += specular * specularColor * envSpecColor;

	// IBL Ambient
	color += indirectColor * diffuseColor * indirectMultiplier / PI;

	// IBL reflection
#if defined(ENABLE_SSR)	
	float brightness = (0.8 + gloss * 1.8);
	float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * brightness * specularColor;
#endif

	return color;
}