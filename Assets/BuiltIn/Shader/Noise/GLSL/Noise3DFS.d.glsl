precision mediump float;

uniform vec4 uNoiseOffset;

in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "LibNoise.glsl"
#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	float n = pnoise(uNoiseOffset.xyz + varWorldPos.xyz * uNoiseOffset.w);
	n = 0.5 + 0.5 * n;
	vec4 ret = varColor * vec4(n, n, n, 1.0);
	FragColor = vec4(sRGB(ret.rgb), ret.a);
}