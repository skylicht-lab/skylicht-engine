precision mediump float;

uniform sampler2D uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

void main(void)
{
	vec4 color = texture(uTexDiffuse, varTexCoord0.xy) * varColor;
	lowp float lum = 0.21 * color.r + 0.72 * color.g + 0.07 * color.b;
	FragColor = vec4(lum, lum, lum, color.a);
}