precision mediump float;

uniform sampler2D uTexture;

in vec2 varTexCoord0;
in vec4 varColor;

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec4 color = texture(uTexture, varTexCoord0.xy);	
	FragColor = vec4(sRGB(varColor.rgb), color.r * varColor.a);
}