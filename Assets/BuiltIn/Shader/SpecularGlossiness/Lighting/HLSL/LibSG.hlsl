static const float PI = 3.1415926;

#include "LibSolverMetallic.hlsl"
#if defined(ENABLE_SSR)
#include "../../../SSR/HLSL/LibSSR.hlsl"
#endif
#include "../../../PostProcessing/HLSL/LibToneMapping.hlsl"

float3 SG(
	const float3 baseColor,
	const float spec,
	const float gloss,
	const float4 position,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 lightColor,
	const float visibility,
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
	float3 directionLightColor = sRGB(lightColor);
	float3 pointLightColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);

	float c = (1.0 - spec * gloss);

	// Lighting
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);

	// Specular
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;

	float3 envSpecColor = lerp(indirectColor, float3(1.0, 1.0, 1.0), visibility);
	float3 directionalLight = NdotL * directionLightColor * visibility;
	
	// Direction lighting
	float3 color = (directionalLight * directMultiplier) * diffuseColor * (0.1 + roughness * 0.3) * c;
	color += pointLightColor * lightMultiplier * diffuseColor * 0.5;
	
	// Direction specular
	color += specular * specularColor * envSpecColor;

	// IBL Ambient
	color += indirectColor * diffuseColor * indirectMultiplier / PI;
	
#if defined(ENABLE_SSR)	
	// IBL reflection
	float brightness = (0.8 + gloss * 1.8);
	float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * brightness * specularColor;
#endif

	return color;
}