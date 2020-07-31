precision mediump float;

uniform sampler2DArray uTexLightmap;

in vec3 varTexCoord0;

out vec4 FragColor;

void main(void)
{
	FragColor = texture(uTexLightmap, varTexCoord0) * 1.6;
}