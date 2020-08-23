precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2DArray uTexLightmap;

in vec3 varTexCoord0;

out vec4 FragColor;

void main(void)
{
	FragColor = texture(uTexLightmap, varTexCoord0);
}