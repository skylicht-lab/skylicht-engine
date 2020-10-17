#define TONE_MAPPING
//#define ACES
//#define FILMIC
//#define UNCHARDTED2
//#define UNREAL

static const float gamma = 2.2;
static const float invGamma = 1.0 / 2.2;

#if defined(ACES)
// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
float3 aces(float3 x) {
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
float3 filmic(float3 x) {
	float3 X = max(float3(0.0, 0.0, 0.0), x - 0.004);
	float3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
	return result;
}
#endif

#if defined(UNCHARDTED2)
float3 uncharted2Tonemap(float3 x) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float3 uncharted2(float3 color) {
	const float W = 11.2;
	float exposureBias = 2.0;
	float3 curr = uncharted2Tonemap(exposureBias * color);
	float3 whiteScale = 1.0 / uncharted2Tonemap(float3(W, W, W));
	return curr * whiteScale;
}
#endif

#if defined(UNREAL)
float3 unreal(float3 x) {
	return x / (x + 0.155) * 1.019;
}
#endif

#if defined(TONE_MAPPING)
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}

float3 linearRGB(float3 color)
{
#if defined(ACES)
	return pow(aces(color), invGamma);
#elif defined(FILMIC)	
	return filmic(color);
#elif defined(UNCHARDTED2)
	return pow(uncharted2(color), invGamma);
#elif defined(UNREAL)
	return unreal(color);
#else	
	return pow(color, invGamma);
#endif
}

#else

#define sRGB(c) (c)
#define linearRGB(c) (c)

#endif