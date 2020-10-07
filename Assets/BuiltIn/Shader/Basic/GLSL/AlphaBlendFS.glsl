precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexAlpha;

in vec2 varTexCoord0;
in vec4 varColor;

out vec4 FragColor;

void main(void)
{
	vec4 t1 = texture(uTexDiffuse, varTexCoord0.xy) * varColor;
	vec4 t2 = texture(uTexAlpha, varTexCoord0.xy);

	t1.a = t2.r * varColor.a;

	FragColor = t1;
}