precision mediump float;

uniform sampler2D uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

#include "LibToneMapping.glsl"

void main(void)
{
	float4 color = texture(uTexDiffuse, varTexCoord0.xy);
	FragColor = float4(linearRGB(color.rgb), color.a);
}