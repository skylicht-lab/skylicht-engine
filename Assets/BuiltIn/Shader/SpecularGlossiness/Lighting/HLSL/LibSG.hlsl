//static const float EnvironmentScale = 3.0;
//static const float PI = 3.1415926;
static const float MinReflectance = 0.04;

float getPerceivedBrightness(float3 color)
{
    return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_materials_pbrSpecularGlossiness/examples/convert-between-workflows/js/three.pbrUtilities.js#L34
float solveMetallic(float3 diffuse, float3 specular, float oneMinusSpecularStrength) 
{
    float specularBrightness = getPerceivedBrightness(specular);
    float diffuseBrightness = getPerceivedBrightness(diffuse);
	
    float a = MinReflectance;
    float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
    float c = MinReflectance - specularBrightness;
    float D = b * b - 4.0 * a * c;

    return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

float3 SG(
	const float3 baseColor, 
	const float spec, 
	const float gloss,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 ambient,
	const float3 lightColor)
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
	float3 diffuseColor = baseColor.rgb * (float3(1.0, 1.0, 1.0) - f0) * (1.0 - metallic);
    specularColor = lerp(f0, baseColor.rgb, metallic);
	
	// Lighting
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	
	// Specular		
	float3 H = normalize(worldLightDir + worldViewDir);	
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
		
	float3 color = (ambient + NdotL * lightColor) * (diffuseColor + specular * specularColor * 4.0);
		
	// IBL Ambient
	// color += IBLAmbient(worldNormal) * diffuseColor / PI * EnvironmentScale;	
	
	// IBL Reflection
	// float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	// color += uTexReflect.SampleLevel(uTexReflectSampler, reflection, roughness*uMipmapCount).rgb * specularColor * (1.5 + metallic);
	
	return color;
}