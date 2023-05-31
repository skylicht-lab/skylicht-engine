// thanks savegame (https://github.com/skylicht-lab/skylicht-engine/issues/130)
#define RAND(co) (float2(frac(sin(dot(co.xy, kRandom1)) * kRandom2), frac(sin(dot(co.yx, kRandom1)) * kRandom2)) * kRandom3)
#define COMPARE(uv, compare) (step(compare, uShadowMap.SampleLevel(uShadowMapSampler, uv, 0).r))

// #define HARD_SHADOW
// #define PCF_NOISE

#if defined(PCF_NOISE)
#define SHADOW_SAMPLE(x, y) {\
off = float2(x, y) * size;\
rand = uv + off;\
rand += RAND(rand);\
result += COMPARE(rand, depth);\
}
#else
#define SHADOW_SAMPLE(x, y) result += COMPARE(uv + float2(x, y) * size, depth);
#endif

float shadowSimple(const float4 shadowCoord)
{
	int id = 0;
	float visible = 1.0;	
	const float bias = 0.0001;
	float depth = 0.0;

	float result = 0.0;

	float3 shadowUV = shadowCoord.xyz / shadowCoord.w;

	depth = shadowUV.z;
	depth -= bias;
	
	float2 uv = shadowUV.xy;
	
#if defined(HARD_SHADOW)
	return COMPARE(uv, depth);
#else
	
	float size = 1.0/2048;
	
#if defined(PCF_NOISE)
	float2 off;
	float2 rand;
	
	const float2 kRandom1 = float2(12.9898,78.233);
	const float kRandom2 = 43758.5453;
	const float kRandom3 = 0.00047;
#endif

	SHADOW_SAMPLE(-1, -1)
	SHADOW_SAMPLE( 0, -1)
	SHADOW_SAMPLE( 1, -1)
	
	SHADOW_SAMPLE(-1, 0)
	SHADOW_SAMPLE( 0, 0)
	SHADOW_SAMPLE( 1, 0)
	
	SHADOW_SAMPLE(-1, 1)
	SHADOW_SAMPLE( 0, 1)
	SHADOW_SAMPLE( 1, 1)

	return result / 9.0;
#endif
}