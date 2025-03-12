precision mediump float;

uniform sampler2D uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;

uniform vec4 uColorIntensity;

out vec4 FragColor;

void main(void)
{
	vec4 color = texture(uTexDiffuse, varTexCoord0.xy) * varColor * uColorIntensity;
	FragColor = color * varColor.a;
}