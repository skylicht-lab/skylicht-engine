precision mediump float;

uniform sampler2D uTexture;

in vec2 varTexCoord0;
in vec4 varColor;

out vec4 FragColor;

void main(void)
{
	vec4 color = texture(uTexture, varTexCoord0.xy) * varColor;
	FragColor = color;
}