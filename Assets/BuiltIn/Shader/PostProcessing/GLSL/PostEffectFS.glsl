precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexColor;
uniform sampler2D uTexCurrentLum;
uniform sampler2D uTexLastLum;
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
	float currentLum = textureLod(uTexCurrentLum, varTexCoord0.xy, 10.0).x;
	if (varTexCoord0.x < 0.5)
	{
		float c = currentLum;
		FragColor = vec4(c, c, c, 1.0);
		return;
	}
	FragColor = vec4(linearRGB(color.rgb), color.a);
}
