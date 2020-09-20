precision mediump float;

uniform vec4 uNoiseOffset;

in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "LibNoise.glsl"

void main(void)
{
	float n = pnoise(uNoiseOffset.xyz + varWorldPos.xyz * uNoiseOffset.w);
	
	FragColor = varColor * vec4(n, n, n, 1.0);
}