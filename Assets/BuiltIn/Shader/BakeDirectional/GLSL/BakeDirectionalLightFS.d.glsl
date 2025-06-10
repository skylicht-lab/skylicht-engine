precision mediump float;

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

uniform vec4 uLightColor;
uniform vec4 uLightDirection;
uniform vec4 uShadowBias;
uniform mat4 uShadowMatrix;

out vec4 FragColor;

#include "../../Shadow/GLSL/LibShadowSimple.glsl"

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

	vec4 shadowCoord = uShadowMatrix * vec4(varWorldPosition, 1.0);
	float visibility = shadowSimple(shadowCoord);
	
	float NdotL = max(dot(worldNormal, uLightDirection.xyz), 0.0);
	vec3 directionalLightColor = NdotL * uLightColor.rgb / 3.0;
	FragColor = vec4(directionalLightColor * visibility, visibility);
}