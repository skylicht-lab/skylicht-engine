precision mediump float;

uniform sampler2D uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec4 result = texture(uTexDiffuse, varTexCoord0.xy) * varColor;
	FragColor = vec4(sRGB(result.rgb), result.a);
}