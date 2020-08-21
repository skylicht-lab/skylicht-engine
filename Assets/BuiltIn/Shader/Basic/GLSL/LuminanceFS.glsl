precision mediump float;

uniform sampler2D uTexDiffuse;

in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;

void main(void)
{
	vec4 color = texture(uTexDiffuse, varTexCoord0.xy) * varColor;
	float lum = max(dot(color.rgb, vec3(0.299, 0.587, 0.114)), 0.0001);
	FragColor = vec4(lum, 0.0, 0.0, 0.0);
}