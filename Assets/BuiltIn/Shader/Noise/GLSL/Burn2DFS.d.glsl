precision highp float;

uniform vec4 uNoiseOffset;

in vec4 varColor;
in vec2 varTexCoord0;

out vec4 FragColor;

#include "LibNoise.glsl"

float snoise( in vec3 x )
{
	return noise(x) * 2.0 - 1.0;
}

void main(void)
{
	int ITERATIONS = 13;
	
	float uSPEED = 10.0;
	float uDISPLACEMENT = 0.05;
	float uTIGHTNESS = 10.0;
	float uYOFFSET = 0.3;
	float uYSCALE = 0.2;
	float uXSCALE = 0.5;
	vec3 uFLAMETONE = vec3(50.0, 5.0, 1.0);
	
	float nx = 0.0;
	float ny = 0.0;
	
	float time = uNoiseOffset.x;
	vec2 uv = varTexCoord0;
	vec2 uvNoise = varTexCoord0 * vec2(uXSCALE, uYSCALE);
	
	for (int i=4; i<ITERATIONS+1; i++)
	{
		float ii = pow(i, 2.0);
		float ifrac = float(i)/float(ITERATIONS);
		
		float t = ifrac * time * uSPEED;
		float d = (1.0-ifrac) * uDISPLACEMENT;
		
		nx += snoise( vec3(uvNoise.x*ii-time*ifrac, uvNoise.y*ii-t, 0.0)) * d * 2.0;
		ny += snoise( vec3(uvNoise.x*ii+time*ifrac, uvNoise.y*ii-t, time*ifrac/ii)) * d;
	}
	
	vec2 pos = vec2(uv.x+nx, uv.y+ny);	
	
	// float flame = clamp(1.0 - pos.y+uYOFFSET, 0.0, 1.0);
	float flame = clamp(sin(pos.x*3.1416) - pos.y+uYOFFSET, 0.0, 1.0);
	
	vec3 col = pow(flame, uTIGHTNESS) * uFLAMETONE;
    
    // tonemapping
    col = col / (1.0+col);
	float e = 1.0/2.2;
    col = pow(col, vec3(e, e, e));
    col = clamp(col, 0.0, 1.0);
	
	FragColor = varColor * vec4(col, 1.0);
}