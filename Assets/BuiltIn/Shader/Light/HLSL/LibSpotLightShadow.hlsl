#define SHADOW_SAMPLE(x, y, z) {\
fragToLight = -lightDir + float3(x, y, z);\
shadow += step(uPointLightShadowMap.SampleLevel(uPointLightShadowMapSampler, fragToLight, 0).r, d);\
}

float3 spotlightShadow(
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
		attenuation *= pow(spotValue, lightAttenuation.w);
	}
	
	float NdotL = max(0.0, dot(lightDir, normal));

	// Specular
	float3 v = camPosition - position;
	float3 viewDir = normalize(v);
	
	float3 H = normalize(direction + viewDir);
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;

	// Shadow
	float bias = 0.2;
	float d = distance - bias;

#if defined(HARD_SHADOW)
	float sampledDistance = uPointLightShadowMap.SampleLevel(uPointLightShadowMapSampler, -lightDir, 0).r;
	float shadow = step(sampledDistance, d);
#else
	float shadow = 0.0;
	float samples = 2.0;
	float offset = 0.01;
	float delta = offset / (samples * 0.5);
	float3 fragToLight;
	
	float x = -offset;
	float y = -offset;
	float z = -offset;
	
	SHADOW_SAMPLE(x, y, z);
	z += delta;
	SHADOW_SAMPLE(x, y, z);
	z = -offset;
	
	y += delta;
	SHADOW_SAMPLE(x, y, z);
	z += delta;
	SHADOW_SAMPLE(x, y, z);
	
	x += delta;
	y = -offset;
	z = -offset;
	
	SHADOW_SAMPLE(x, y, z);
	z += delta;
	SHADOW_SAMPLE(x, y, z);
	z = -offset;
	
	y += delta;
	SHADOW_SAMPLE(x, y, z);
	z += delta;
	SHADOW_SAMPLE(x, y, z);
	
	shadow /= (samples * samples * samples);
#endif

	shadow = max(1.0 - shadow, 0.0);

	return (lightColor.rgb * NdotL + specular * specColor) * attenuation * shadow;
}