#define SHADOW_SAMPLE(x, y, z) {\
fragToLight = -lightDir + vec3(x, y, z);\
shadow += step(textureLod(uPointLightShadowMap, fragToLight, 0.0).r, d);\
}

vec3 pointlightShadow(
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
	vec3 direction = lightPosition.xyz - position;
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
	vec3 fragToLight;
	
	/*
	for (float x = -offset; x < offset; x += delta)
	{
		for (float y = -offset; y < offset; y += delta)
		{
			for (float z = -offset; z < offset; z += delta)
			{
				SHADOW_SAMPLE(x, y, z)
			}
		}
	}
	*/	
	
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

	return (lightColor.rgb * NdotL + specular * specColor) * shadow * attenuation;
}
