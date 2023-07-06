Texture2D uTexPosition : register(t0);
SamplerState uTexPositionSampler : register(s0);

Texture2D uTexNormal : register(t1);
SamplerState uTexNormalSampler : register(s1);

Texture2D uTexData : register(t2);
SamplerState uTexDataSampler : register(s2);

TextureCube uShadowMap : register(t3);
SamplerState uShadowMapSampler : register(s3);

// #define HARD_SHADOW

#define SHADOW_SAMPLE(x, y, z) {\
fragToLight = -lightDir + float3(x, y, z);\
shadow += step(uShadowMap.SampleLevel(uShadowMapSampler, fragToLight, 0).r, d);\
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightPosition;
	float4 uLightAttenuation;
	float4 uLightColor;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 position = uTexPosition.Sample(uTexPositionSampler, input.tex0).xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;

	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);

	float spec = data.r;
	float gloss = data.g;

	// Lighting	
	float3 direction = uLightPosition.xyz - position;
	float distance = length(direction);
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y)) * uLightColor.a;

	float3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));

	// Specular
	float3 H = normalize(direction + viewDir);
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;

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
	
	/*
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

	float3 lightColor = uLightColor.rgb * (NdotL * attenuation) * shadow;
	return float4(lightColor, specular * attenuation * shadow);
}