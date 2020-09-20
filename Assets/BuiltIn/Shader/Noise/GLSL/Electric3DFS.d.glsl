precision mediump float;

uniform vec4 uNoiseOffset;

in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "LibNoise.glsl"

void main(void)
{
	float rz = fbm(uNoiseOffset.xyz + varWorldPos.xyz * uNoiseOffset.w);
	
	rz *= 2.0f;
	
	vec3 col = vec3(.2, 0.1, 0.4) / rz;	
	
	FragColor = varColor * vec4(col, 1.0);
}