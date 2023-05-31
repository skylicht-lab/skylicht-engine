precision mediump float;

uniform sampler2D uShadowMap;

in vec3 varWorldPosition;
in vec3 varWorldNormal;

uniform vec4 uLightColor;
uniform vec4 uLightDirection;
uniform mat4 uShadowMatrix;

out vec4 FragColor;

#include "../../Shadow/GLSL/LibShadowSimple.glsl"

void main(void)
{
	vec4 shadowCoord = uShadowMatrix * vec4(varWorldPosition, 1.0);
	float visibility = shadowSimple(shadowCoord);
	
	float NdotL = max(dot(varWorldNormal, uLightDirection.xyz), 0.0);
	vec4 directionalLightColor = NdotL * uLightColor;
	FragColor = vec4(directionalLightColor.rgb * visibility, 1.0);
}