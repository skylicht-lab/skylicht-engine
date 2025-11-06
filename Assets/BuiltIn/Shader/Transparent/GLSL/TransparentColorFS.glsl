precision mediump float;

uniform sampler2D uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

uniform vec4 uColor;
uniform vec4 uColorIntensity;

void main(void)
{
	FragColor = texture(uTexDiffuse, varTexCoord0.xy) * varColor * uColor * uColorIntensity;
}