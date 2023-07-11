precision mediump float;

uniform sampler2D uDirectionalLight;
uniform sampler2D uPointLight;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

void main(void)
{
	vec4 l1 = texture(uDirectionalLight, varTexCoord0.xy);
	vec4 l2 = texture(uPointLight, varTexCoord0.xy);
	
	FragColor = vec4(l1.rgb + l2.rgb, l1.a);
}