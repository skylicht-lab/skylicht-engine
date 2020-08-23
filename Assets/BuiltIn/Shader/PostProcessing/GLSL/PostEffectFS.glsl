precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexColor;
uniform sampler2D uTexLuminance;
in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;
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
void main(void)
{
	vec4 color = texture(uTexColor, varTexCoord0.xy);
	float avgLuminance = textureLod(uTexLuminance, varTexCoord0.xy, 10.0).x;
	float target = 0.25;
	float threshold = 2.5;
	float linearExposure = max((target / avgLuminance), 0.0001);
	float exposure = min(exp2(log2(linearExposure)), threshold);
	FragColor = vec4(linearRGB(exposure * color.rgb), color.a);
}
