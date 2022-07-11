// thanks savegame (https://github.com/skylicht-lab/skylicht-engine/issues/130)
#define RAND(co) (vec2(fract(sin(dot(co.xy, kRandom1)) * kRandom2), fract(sin(dot(co.yx, kRandom1)) * kRandom2)) * kRandom3)
#define COMPARE(uv, compare) (step(compare, texture(uShadowMap, uv).r))

#define SHADOW_SAMPLE(x, y) {\
off = vec2(x, y) / size;\
rand = uv + off;\
rand += RAND(rand);\
result += COMPARE(vec3(rand, id), depth);\
}

float shadow(const vec4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{
	int id = 0;
	float visible = 1.0;
	float bias = 0.0001;
	float depth = 0.0;

	float result = 0.0;
	float size = 2048.0;

	if (farDistance < shadowDistance[0])
		id = 0;
	else if (farDistance < shadowDistance[1])
		id = 1;
	else if (farDistance < shadowDistance[2])
		id = 2;
	else
		return 1.0;

	vec3 shadowUV = shadowCoord[id].xyz / shadowCoord[id].w;

	depth = shadowUV.z;
	depth -= bias;
	
	vec2 uv = shadowUV.xy;
	vec2 off;
	vec2 rand;
	
	const vec2 kRandom1 = vec2(12.9898,78.233);
	const float kRandom2 = 43758.5453;
	const float kRandom3 = 0.00047;

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
}