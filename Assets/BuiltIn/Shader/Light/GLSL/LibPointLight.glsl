vec3 pointlight(
	const vec3 position, 
	const vec3 normal,
	const vec3 camPosition, 
	const vec4 lightColor, 
	const vec3 lightPosition, 
	const vec4 lightAttenuation, 
	const float spec, 
	const float gloss, 
	const vec3 specColor)
{
	// Lighting	
	vec3 direction = lightPosition - position;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * lightAttenuation.y)) * lightColor.a;

	vec3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));

	// Specular
	vec3 v = camPosition.xyz - position;
	vec3 viewDir = normalize(v);
	vec3 H = normalize(direction + viewDir);
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0 * gloss) * spec;	

	return (lightColor.rgb * NdotL + specular * specColor) * attenuation;
}
