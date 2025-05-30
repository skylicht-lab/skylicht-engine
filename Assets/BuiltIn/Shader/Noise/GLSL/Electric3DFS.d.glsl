precision highp float;

uniform vec4 uNoiseOffset;
uniform vec4 uElectricColor;

in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "LibNoise.glsl"
#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	float f = pnoise(uNoiseOffset.xyz + varWorldPos.xyz * uNoiseOffset.w);

	f = abs(f + 0.1);
	f = pow(f, 0.2);

	vec3 col = vec3(1.7, 1.7, 1.7);
	col = col * -f + col;
	col = col * col;
	col = col * col;
	col = col * uElectricColor.rgb;

	vec4 ret = varColor * vec4(col, 1.0);
	FragColor = vec4(sRGB(ret.rgb), ret.a);
}