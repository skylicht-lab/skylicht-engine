float3 spotlight(
	const float3 position, 
	const float3 normal, 
	const float3 camPosition, 
	const float4 lightColor, 
	const float3 lightPosition, 
	const float4 lightAttenuation, 
	const float spec, 
	const float gloss, 
	const float3 specColor)
{
	// Lighting	
	float3 direction = lightPosition - position;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * lightAttenuation.z)) * lightColor.a;
	
	float3 lightDir = normalize(direction);
	
	float spotDot = dot(lightDir, uLightDirection.xyz);
	if (spotDot < lightAttenuation.x)
	{
		attenuation = 0.0;
	}
	else
	{
		float spotValue = smoothstep(lightAttenuation.x, lightAttenuation.y, spotDot);
		attenuation = pow(spotValue, lightAttenuation.w);
	}
	
	float NdotL = max(0.0, dot(lightDir, normal));

	// Specular
	float3 v = camPosition - position;
	float3 viewDir = normalize(v);
	
	float3 H = normalize(direction + viewDir);
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0 * gloss) * spec;

	return (lightColor.rgb * NdotL + specular * specColor) * attenuation;
}