precision mediump float;

in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "LibNoise.glsl"

void main(void)
{
	float n = fbm(varWorldPos.xyz * 8.0);
	
	FragColor = varColor * vec4(n, n, n, 1.0);
}