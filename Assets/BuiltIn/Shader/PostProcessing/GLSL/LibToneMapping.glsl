#define TONE_MAPPING
//#define ACES
//#define FILMIC
//#define UNCHARDTED2
//#define UNREAL

const float gamma = 2.2;
const float invGamma = 1.0 / 2.2;

#if defined(ACES)
// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 aces(vec3 x) {
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}
#endif

#if defined(FILMIC)
// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 filmic(vec3 x) {
	vec3 X = max(vec3(0.0, 0.0, 0.0), x - 0.004);
	vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
	return result;
}
#endif

#if defined(UNCHARDTED2)
vec3 uncharted2Tonemap(vec3 x) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2(vec3 color) {
	const float W = 11.2;
	float exposureBias = 2.0;
	vec3 curr = uncharted2Tonemap(exposureBias * color);
	vec3 whiteScale = 1.0 / uncharted2Tonemap(vec3(W));
	return curr * whiteScale;
}
#endif

#if defined(UNREAL)
vec3 unreal(vec3 x) {
	return x / (x + 0.155) * 1.019;
}
#endif

vec3 sRGB(vec3 color)
{
	return pow(color, vec3(gamma));
}

vec3 linearRGB(vec3 color)
{
#if defined(ACES)
	return pow(aces(color), vec3(invGamma));
#elif defined(FILMIC)
	return filmic(color);
#elif defined(UNCHARDTED2)
	return pow(uncharted2(color), vec3(invGamma));
#elif defined(UNREAL)
	return unreal(color);
#else
	return pow(color, vec3(invGamma));
#endif
}

