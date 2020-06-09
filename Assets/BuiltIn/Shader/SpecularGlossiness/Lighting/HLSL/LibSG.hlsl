//static const float EnvironmentScale = 3.0;
static const float PI = 3.1415926;

#include "LibSolverMetallic.hlsl"

float3 SG(
	const float3 baseColor, 
	const float spec, 
	const float gloss,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 lightColor,
	const float visibility,
	const float4 light,
	const float3 indirect)
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
	
	// Lighting
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	
	// Specular
	float3 H = normalize(worldLightDir + worldViewDir);	
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	
	float3 directionalLight = NdotL * lightColor * visibility;
	float3 color = (directionalLight + light.rgb) + (specular * specularColor * visibility + light.a * specularColor);
	
	// IBL Ambient
	color += indirect * diffuseColor / PI;
	
	// IBL Reflection
	// float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	// color += uTexReflect.SampleLevel(uTexReflectSampler, reflection, roughness*uMipmapCount).rgb * specularColor * (1.5 + metallic);
	
	return color * diffuseColor;
}