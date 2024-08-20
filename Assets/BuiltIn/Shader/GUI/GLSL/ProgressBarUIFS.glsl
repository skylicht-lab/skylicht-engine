precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexTiling;
uniform vec4 uColor;

in vec2 vTexCoord0;
in vec2 vTexCoord1;
in vec4 vColor;

out vec4 FragColor;

void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy);
	vec4 tileMap = texture(uTexTiling, vTexCoord1.xy);
	FragColor = vec4(tileMap.rgb * uColor.rgb * vColor.rgb, diffuseMap.a);
}