precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexShiny;
uniform vec4 uColor;

in vec2 vTexCoord0;
in vec2 vTexCoord1;
in vec4 vColor;

out vec4 FragColor;

void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy);
	vec4 shinyMap = texture(uTexShiny, vTexCoord1.xy);
	vec3 color = diffuseMap.rgb * uColor.rgb * vColor.rgb;
	vec3 result = color + shinyMap.rgb;
	FragColor = vec4(result, diffuseMap.a * vColor.a);
}