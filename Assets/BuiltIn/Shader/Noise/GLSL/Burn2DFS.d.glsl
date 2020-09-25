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
	float color = 1.0 - varTexCoord0.y + 0.4;
    
    vec3 coord = vec3(varTexCoord0.x/6.0, varTexCoord0.y/6.0, 0.5);
	
    coord = 1.0 - coord;
    
	float speedMul = 0.05;
	vec3 noiseOffset = vec3(0.0, uNoiseOffset.x, uNoiseOffset.x) * speedMul;
	
    int i = 1;
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	{
        float power = pow(2.0, float(i));
        color += (0.4 / power) * snoise((coord - noiseOffset) * power * uNoiseOffset.w);
		i++;
    }
	
    color = 1.0 - color;
    color *= 2.7;
    
	vec3 col = vec3(color, pow(max(color,0.),2.)*0.4, pow(max(color,0.),3.0)*0.15);
	
	FragColor = varColor * vec4(col, 1.0);
}