precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2DArray uTexIndirect;
uniform sampler2DArray uTexDirectional;

in vec3 varTexCoord0;

layout(location = 0) out vec4 Indirect;
layout(location = 1) out vec4 Directional;

void main(void)
{
	Indirect = texture(uTexIndirect, varTexCoord0) * 3.0;
	Directional = texture(uTexDirectional, varTexCoord0) * 3.0;
}