TextureCube uShadowMap : register(t0);
SamplerState uShadowMapSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldPosition: WORLDPOSITION;
	float3 tex0 : LIGHTMAP;
};

cbuffer cbPerFrame
{
	float4 uLightPosition;
	float4 uLightAttenuation;
	float4 uLightColor;
};

#define SHADOW_SAMPLE(x, y, z) {\
fragToLight = -lightDir + float3(x, y, z);\
shadow += step(uShadowMap.SampleLevel(uShadowMapSampler, fragToLight, 0).r, d);\
}

float4 main(PS_INPUT input) : SV_TARGET
{
	// Lighting
	float3 direction = uLightPosition.xyz - input.worldPosition;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y)) * uLightColor.a;

	float3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, input.worldNormal));
	
	// Shadow
	float bias = 0.2;
	float d = distance - bias;

#if defined(HARD_SHADOW)
	float sampledDistance = uShadowMap.SampleLevel(uShadowMapSampler, -lightDir, 0).r;
	float shadow = step(sampledDistance, d);
#else
	float shadow = 0.0;
	float samples = 2.0;
	float offset = 0.01;
	float delta = offset / (samples * 0.5);
	float3 fragToLight;	
	
	[unroll]
	for (float x = -offset; x < offset; x += delta)
	{
		[unroll]
		for (float y = -offset; y < offset; y += delta)
		{
			[unroll]
			for (float z = -offset; z < offset; z += delta)
			{
				SHADOW_SAMPLE(x, y, z)
			}
		}
	}
	
	shadow /= (samples * samples * samples);
#endif

	float3 directionalLightColor = NdotL * attenuation * uLightColor.rgb * shadow;
	return float4(directionalLightColor / 3.0, 1.0);
}