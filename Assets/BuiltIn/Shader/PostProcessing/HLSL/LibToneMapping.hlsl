#define TONE_MAPPING

static const float gamma = 2.2;
static const float invGamma = 1.0/2.2;

float3 sRGB(float3 color)
{
	return pow(color, gamma);
}

float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}

