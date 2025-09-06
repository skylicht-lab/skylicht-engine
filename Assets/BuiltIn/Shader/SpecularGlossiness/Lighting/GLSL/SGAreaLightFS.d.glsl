precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;

uniform vec4 uCameraPosition;
uniform vec4 uLightPosition;
uniform vec3 uLightDirX;
uniform vec3 uLightDirY;
uniform vec2 uLightSize;
uniform vec4 uLightColor;

in vec2 varTexCoord0;

out vec4 FragColor;

#include "../../../Light/GLSL/LibAreaLight.glsl"

void main(void)
{
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;

	vec3 lightColor = arealight(
		position, 
		normal,
		uCameraPosition.xyz, 
		uLightColor, 
		uLightPosition.xyz, 
		uLightDirX, 
		uLightDirY,
		uLightSize);

	FragColor = vec4(lightColor, 1.0);
}