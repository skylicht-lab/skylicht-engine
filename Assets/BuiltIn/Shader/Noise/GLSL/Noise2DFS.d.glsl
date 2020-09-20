precision highp float;

uniform vec4 uNoiseOffset;

in vec4 varColor;
in vec2 varTexCoord0;

out vec4 FragColor;

#include "LibNoise2D.glsl"

void main(void)
{
	// vec3 n = noised(varTexCoord0 * uNoiseOffset.xy);
	// n = 0.5 + 0.5*n;		
	// FragColor = varColor * vec4(n.yzx, 1.0);
	
	float f = pnoise(varTexCoord0 * 8.0);
	f = 0.5 + 0.5*f;	
	FragColor = varColor * vec4(f, f, f, 1.0);
}