precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;

uniform vec4 uCameraPosition;
uniform vec4 uLightPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightAttenuation;
uniform vec4 uLightColor;

in vec2 varTexCoord0;

out vec4 FragColor;

#include "../../../Light/GLSL/LibSpotLight.glsl"

void main(void)
{
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;

	vec3 lightColor = spotlight(
		position, 
		normal,
		uCameraPosition.xyz, 
		uLightColor, 
		uLightPosition.xyz, 
		uLightAttenuation, 
		data.r, // spec
		data.g, // gloss 
		vec3(1.0, 1.0, 1.0));

	FragColor = vec4(lightColor, 1.0);
}