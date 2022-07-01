precision mediump float;

in vec4 varColor;

uniform vec4 uAmbientColor;

out vec4 FragColor;

void main(void)
{
	FragColor = uAmbientColor;
}