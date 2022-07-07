precision mediump float;

uniform sampler2D uTexDiffuse;
uniform vec4 uColor;

in vec2 vTexCoord0;

out vec4 FragColor;

void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy);
	
	FragColor = diffuseMap * uColor;
}