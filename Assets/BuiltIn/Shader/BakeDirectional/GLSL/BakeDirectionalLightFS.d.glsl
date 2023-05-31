precision mediump float;

in vec3 varWorldPosition;
in vec3 varWorldNormal;

uniform vec4 uLightColor;
uniform vec4 uLightDirection;

out vec4 FragColor;

void main(void)
{
	float NdotL = max(dot(varWorldNormal, uLightDirection.xyz), 0.0);
	vec4 directionalLightColor = NdotL * uLightColor;
	FragColor = vec4(directionalLightColor.rgb, 1.0);
}