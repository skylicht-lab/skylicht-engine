const float EnvironmentScale = 3.0;
const float PI = 3.1415926;
const float MinReflectance = 0.04;

float getPerceivedBrightness(vec3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_materials_pbrSpecularGlossiness/examples/convert-between-workflows/js/three.pbrUtilities.js#L34
float solveMetallic(vec3 diffuse, vec3 specular, float oneMinusSpecularStrength) 
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);
	
	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;

	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

vec3 SG(
	const vec3 baseColor, 
	const float spec, 
	const float gloss,
	const vec3 worldViewDir,
	const vec3 worldLightDir,
	const vec3 worldNormal,
	const vec3 ambient,
	const vec3 lightColor,
	const float visibility)
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
	
	// Specular
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0,dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	
	vec3 color = (NdotL * lightColor * visibility) * (diffuseColor + specular * specularColor * visibility);
	
	// IBL Ambient
	// color += IBLAmbient(worldNormal) * diffuseColor / PI * EnvironmentScale;
	
	// IBL Reflection
	// vec3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	// color += texture(uTexReflect, reflection, roughness*uMipmapCount).rgb * specularColor * (1.5 + metallic);
	
	return color;
}
