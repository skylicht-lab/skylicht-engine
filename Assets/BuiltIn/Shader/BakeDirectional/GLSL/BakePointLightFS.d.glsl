precision mediump float;

uniform samplerCube uShadowMap;

in vec3 varWorldPosition;
in vec3 varWorldNormal;

uniform vec4 uLightPosition;
uniform vec4 uLightAttenuation;
uniform vec4 uLightColor;

out vec4 FragColor;

#define HARD_SHADOW

#define SHADOW_SAMPLE(x, y, z) {\
fragToLight = -lightDir + vec3(x, y, z);\
shadow += step(texture(uShadowMap, fragToLight).r, d);\
}

void main(void)
{
	// Lighting
	vec3 direction = uLightPosition.xyz - varWorldPosition;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y)) * uLightColor.a;

	vec3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, varWorldNormal));
	
	// Shadow
	float bias = 0.2;
	float d = distance - bias;
	
#if defined(HARD_SHADOW)
	float sampledDistance = texture(uShadowMap, -lightDir).r;
	float shadow = step(sampledDistance, d);
#else
	
	float shadow = 0.0;
	float samples = 2.0;
	float offset = 0.01;
	float delta = offset / (samples * 0.5);
	vec3 fragToLight;
		
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
	
	shadow /= (samples * samples * samples);
#endif
	
	shadow = max(1.0 - shadow, 0.0);
	
	vec3 directionalLightColor = NdotL * attenuation * uLightColor.rgb * shadow;
	FragColor = vec4(directionalLightColor/ 3.0, 1.0);
}