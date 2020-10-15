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
	const float3 position,
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

	// Metallic
	float3 f0 = spec;
	float3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);

	// Color
	f0 = float3(0.04, 0.04, 0.04);
	float3 diffuseColor = baseColor.rgb;
	specularColor = lerp(f0, baseColor.rgb, metallic);

	// Tone mapping
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	float3 directionLightColor = sRGB(lightColor);
	float3 pointLightColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);

	// Lighting
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);

	// Specular
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;

	float3 directionalLight = NdotL * directionLightColor * visibility;
	float3 color = (directionalLight * directMultiplier + pointLightColor * lightMultiplier) * diffuseColor + specular * specularColor * visibility + light.a * specularColor;

	// IBL Ambient
	color += indirectColor * diffuseColor * indirectMultiplier / PI;

	// IBL reflection
#if defined(ENABLE_SSR)	
	float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * metallic * specularColor;
#endif

	return color;
}