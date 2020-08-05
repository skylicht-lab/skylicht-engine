Texture2D uTexPosition : register(t0);
SamplerState uTexPositionSampler : register(s0);

Texture2D uTexNormal : register(t1);
SamplerState uTexNormalSampler : register(s1);

Texture2D uTexData : register(t2);
SamplerState uTexDataSampler : register(s2);

TextureCube uShadowMap : register(t3);
SamplerState uShadowMapSampler : register(s3);

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
	float attenuation = max(0.0, 1.0 - (distance * uLightAttenuation.y));
	
	float3 lightDir = normalize(direction);
	float NdotL = max(0.0, dot(lightDir, normal));
	
	// Specular
	float3 H = normalize(direction + viewDir);	
	float NdotE = max(0.0,dot(normal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	
	// Shadow
	float bias = 0.2;
	
	/*
	float sampledDistance = uShadowMap.SampleLevel(uShadowMapSampler, -lightDir, 0).r;
	float shadow = 1.0;
	if (distance - bias > sampledDistance)
		shadow = 0.0f; // Inside the shadow
	*/
	
	float shadow = 0.0;
	float samples = 3.0;
	float offset = 0.01;
	
	[unroll]
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		[unroll]
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			[unroll]
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float3 fragToLight = -lightDir + float3(x, y, z);
				float closestDepth = uShadowMap.SampleLevel(uShadowMapSampler, fragToLight, 0).r;
				
				if (distance - bias > closestDepth)
					shadow += 1.0;
			}
		}
	}
	
	shadow /= (samples * samples * samples);
	shadow = max(1.0 - shadow, 0.0);
	
	float3 lightColor = uLightColor.rgb * (NdotL * attenuation) * shadow;
	return float4(lightColor, specular * attenuation * shadow);
}