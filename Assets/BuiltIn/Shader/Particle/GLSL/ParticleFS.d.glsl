precision mediump float;

uniform sampler2D uTexture;

in vec2 varTexCoord0;
in vec4 varColor;

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec4 color = texture(uTexture, varTexCoord0.xy);
	vec3 result = sRGB(color.rgb * varColor.rgb);
	FragColor = vec4(result, color.a * varColor.a);
}