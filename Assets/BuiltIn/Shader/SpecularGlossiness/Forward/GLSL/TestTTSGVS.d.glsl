in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inData;
#endif

uniform sampler2D uTransformTexture;

uniform mat4 uVpMatrix;
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uUVScale;
uniform vec2 uTransformTextureSize;
uniform vec2 uTransformXY;

out vec2 vTexCoord0;
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
out vec3 vWorldLightDir;
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
out vec3 vWorldTangent;
out vec3 vWorldBinormal;
out float vTangentW;
#endif
out vec4 vViewPosition;
out vec3 vWorldPosition;

#include "../../../TransformTexture/GLSL/LibTransformTexture.glsl"

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	vTangentW = inData.x;
#endif

	mat4 worldMatrix = uWorldMatrix * getTransformFromTexture(uTransformXY);

	vec4 worldPos = worldMatrix * inPosition;
	vec4 worldViewDir = normalize(uCameraPosition - worldPos);

	vec4 worldNormal = worldMatrix * vec4(inNormal.xyz, 0.0);

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	vec4 worldTangent = worldMatrix * vec4(inTangent.xyz, 0.0);
#endif

	vWorldPosition = worldPos.xyz;
	vWorldNormal = normalize(worldNormal.xyz);

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	vWorldTangent = normalize(worldTangent.xyz);
	vWorldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
#endif

	vWorldViewDir = worldViewDir.xyz;
	vWorldLightDir = normalize(uLightDirection.xyz);
	
	mat4 mvpMatrix = uVpMatrix * worldMatrix;
	vViewPosition = mvpMatrix * inPosition;

	gl_Position = vViewPosition;
}