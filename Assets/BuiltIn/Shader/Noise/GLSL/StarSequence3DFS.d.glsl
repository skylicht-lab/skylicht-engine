// Base on https://www.shadertoy.com/view/4dXGR4

precision mediump float;

uniform vec4 uNoiseOffset;
uniform vec4 uElectricColor;

in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "LibNoise.glsl"
#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

float snoise(vec3 coord)
{
	return 1.0 - noise(coord) * 2.0;
}

void main(void)
{
	vec3 coord = varWorldPos.xyz * uNoiseOffset.w;
	float n = pnoise(uNoiseOffset.xyz + varWorldPos.xyz * 12.0);
	
	coord += vec3(n, 0.0, n);
	
	float color = 0.5;
	float scale = 1.3;
	
	int i = 1;
	{
        float power = pow(2.0, i);
		float res = power * scale;
        color += (0.4 / power) * snoise(coord * res);
    }	
	i++;
	{
        float power = pow(2.0, i);
		float res = power * scale;
        color += (0.4 / power) * snoise(coord * res);
    }
	
	color *= 2.3;
	
	float r = color;
	float g = pow(max(color, 0.0),2.0)*0.4;
	float b = pow(max(color, 0.0),3.0)*0.15;
		
	vec4 ret = varColor * vec4(r, g, b, 1.0);
	FragColor = vec4(sRGB(ret.rgb), ret.a);
}