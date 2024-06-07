precision mediump float;

uniform sampler2D uTexDiffuse;
uniform vec4 uColor;
uniform vec2 uIntensity;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec4 result = textureLod(uTexDiffuse, varTexCoord0.xy, 0.0) * varColor * uColor;
	FragColor = vec4(sRGB(result.rgb * uIntensity.x), result.a);
}