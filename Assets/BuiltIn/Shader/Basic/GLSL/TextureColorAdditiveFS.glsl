precision mediump float;

uniform sampler2D 	uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;

out vec4 FragColor;

void main(void)
{	
	vec4 color = texture(uTexDiffuse, varTexCoord0.xy) * varColor;
	FragColor = color * varColor.a;
}