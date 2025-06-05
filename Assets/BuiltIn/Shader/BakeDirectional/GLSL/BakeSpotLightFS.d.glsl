precision highp float;
precision highp samplerCube;

uniform samplerCube uPointLightShadowMap;

in vec3 varWorldPosition;
in vec3 varWorldNormal;

uniform vec4 uLightPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightAttenuation;
uniform vec4 uLightColor;

out vec4 FragColor;

#include "../../Light/GLSL/LibSpotLightShadow.glsl"

void main(void)
{
	vec3 directionalLightColor = spotlightShadow(
		varWorldPosition, 
		varWorldNormal,
		vec3(0.0, 100.0, 0.0), 
		uLightColor, 
		uLightPosition.xyz, 
		uLightAttenuation, 
		0.0, 
		0.1, 
		vec3(1.0, 1.0, 1.0));
		
	FragColor = vec4(directionalLightColor / 3.0, 1.0);
}