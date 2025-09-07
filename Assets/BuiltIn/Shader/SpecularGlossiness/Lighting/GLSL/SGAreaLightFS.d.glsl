precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;

#ifdef SHADOW
uniform sampler2D uShadowMap;
#endif

uniform vec4 uCameraPosition;
uniform vec4 uLightPosition;
uniform vec3 uLightDirX;
uniform vec3 uLightDirY;
uniform vec2 uLightSize;
uniform vec4 uLightColor;

#ifdef SHADOW
uniform mat4 uShadowMatrix;
#endif

in vec2 varTexCoord0;

out vec4 FragColor;

#ifdef SHADOW
#include "../../../Light/GLSL/LibAreaLightShadow.glsl"
#else
#include "../../../Light/GLSL/LibAreaLight.glsl"
#endif

void main(void)
{
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;

#ifdef SHADOW
	vec3 lightColor = arealightShadow(
#else
	vec3 lightColor = arealight(
#endif
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