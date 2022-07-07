precision mediump float;

uniform sampler2D uTexDiffuse;
uniform vec4 uColor;
uniform vec4 uLightDirection;

in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;

out vec4 FragColor;

void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy);
	
	float NdotL = max(dot(vWorldNormal, uLightDirection.xyz), 0.0);
	
	FragColor = diffuseMap * uColor * NdotL;
}