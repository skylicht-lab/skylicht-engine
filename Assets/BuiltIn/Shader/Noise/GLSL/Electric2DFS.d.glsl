precision highp float;

uniform vec4 uNoiseOffset;
uniform vec4 uElectricColor;

in vec4 varColor;
in vec2 varTexCoord0;

out vec4 FragColor;

#include "LibNoise2D.glsl"

void main(void)
{
	float f = pnoise(uNoiseOffset.xy + varTexCoord0 * uNoiseOffset.w);

	f = abs(f + 0.1);
	f = pow(f, 0.2);

	vec3 col = vec3(1.7, 1.7, 1.7);
	col = col * -f + col;
	col = col * col;
	col = col * col;
	col = col * uElectricColor.rgb;

	FragColor = varColor * vec4(col, 1.0);
}