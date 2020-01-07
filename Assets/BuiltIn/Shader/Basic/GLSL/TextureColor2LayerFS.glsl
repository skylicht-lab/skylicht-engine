precision mediump float;

uniform sampler2D uTexDiffuse1;
uniform sampler2D uTexDiffuse2;

in vec2 varTexCoord0;
in vec2 varTexCoord1;
in vec4 varColor;

out vec4 FragColor;

void main(void)
{
	vec4 color1 = texture(uTexDiffuse1, varTexCoord0.xy);
	vec4 color2 = texture(uTexDiffuse2, varTexCoord1.xy);

	FragColor = color1 * color2 * varColor;
}