precision highp float;

in vec4 vColor;

uniform vec4 uColor;
uniform vec4 uEmissive;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

out vec4 FragColor;

void main(void)
{
	vec3 color = sRGB(vColor.rgb * uColor.rgb) * uEmissive.rgb * uEmissive.a;
	FragColor = vec4(color, 1.0);
}