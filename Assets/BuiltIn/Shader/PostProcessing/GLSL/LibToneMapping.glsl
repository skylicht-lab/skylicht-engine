#define TONE_MAPPING

const float gamma = 2.2;
const float invGamma = 1.0/2.2;

vec3 sRGB(vec3 color)
{
	return pow(color, vec3(gamma));
}

vec3 linearRGB(vec3 color)
{
	return pow(color, vec3(invGamma));
}

