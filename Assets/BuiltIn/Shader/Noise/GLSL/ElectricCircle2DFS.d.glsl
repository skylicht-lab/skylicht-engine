precision highp float;

uniform vec4 uNoiseOffset;
uniform vec4 uElectricColor;

in vec4 varColor;
in vec2 varTexCoord0;

out vec4 FragColor;

#include "LibNoise2D.glsl"

float paintCircle(vec2 uv, vec2 center, float rad) {

	vec2 diff = center - uv;
	float len = length(diff);

	return smoothstep(rad, 0.0, len);
}

void main(void)
{
	float f = pnoise(uNoiseOffset.xy + varTexCoord0 * uNoiseOffset.w);
	float t = 0.2;

	float radius = 0.7;
	vec2 center = vec2(0.5, 0.5);

	float c = (1.0 - paintCircle(varTexCoord0, center, radius));

	// convert to [-1, 1]
	c = -1.0 + c * 2.0;

	f = abs(f * t + c + 0.01) * 0.3;
	f = pow(f, 0.2);

	vec3 col = vec3(1.7, 1.7, 1.7);
	col = col * -f + col;
	col = col * col;
	col = col * col;
	col = col * uElectricColor.rgb;

	FragColor = varColor * vec4(col, 1.0);
}