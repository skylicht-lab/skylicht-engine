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
	float glossiness = max(gloss, 0.01);
	float roughness = 1.0 - glossiness;

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
	float3 directionColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);

	// Specular
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * glossiness) * spec;
	
	// Direction lighting
	float3 color = (directionColor * lightMultiplier) * diffuseColor;
	
	// Direction specular
	float visibility = light.a;
	float3 envSpecColor = lerp(indirectColor, float3(1.0, 1.0, 1.0), visibility);
	color += specular * specularColor * envSpecColor;

	// IBL Ambient
	color += indirectColor * diffuseColor * indirectMultiplier / PI;

	// IBL reflection
#if defined(ENABLE_SSR)	
	float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * metallic * specularColor;
#endif

	return color;
}