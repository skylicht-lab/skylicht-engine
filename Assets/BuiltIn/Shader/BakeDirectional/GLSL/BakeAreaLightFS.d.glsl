precision highp float;
precision highp sampler2D;

uniform sampler2D uShadowMap;

#if defined(NORMAL_MAP)
uniform sampler2D uTexNormal;
#endif

in vec2 varTex0;
in vec3 varWorldPosition;
in vec3 varWorldNormal;

#if defined(NORMAL_MAP)
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
#endif

uniform vec4 uLightPosition;
uniform vec3 uLightDirX;
uniform vec3 uLightDirY;
uniform vec2 uLightSize;
uniform vec4 uLightColor;
uniform mat4 uShadowMatrix;

out vec4 FragColor;

#include "../../Light/GLSL/LibAreaLightShadow.glsl"

void main(void)
{
#if defined(NORMAL_MAP)
	vec3 normalMap = texture(uTexNormal, varTex0).xyz;
	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, varWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y *= vTangentW;
	vec3 worldNormal = normalize(rotation * localCoords);
#else
	vec3 worldNormal = varWorldNormal;
#endif

	vec3 directionalLightColor = arealightShadow(
		varWorldPosition, 
		worldNormal,
		vec3(0.0, 100.0, 0.0), 
		uLightColor, 
		uLightPosition.xyz, 
		uLightDirX, 
		uLightDirY,
		uLightSize);
		
	FragColor = vec4(directionalLightColor / 3.0, 1.0);
}