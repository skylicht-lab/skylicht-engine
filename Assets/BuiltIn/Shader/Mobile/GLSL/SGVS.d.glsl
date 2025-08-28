#ifdef INSTANCING
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;
layout(location = 6) in vec2 inData;

layout(location = 7) in vec4 uUVScale;
layout(location = 8) in vec4 uColor;
layout(location = 9) in vec4 uSpecGloss;

layout(location = 10) in mat4 uWorldMatrix;
#else
in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inData;
#endif
#endif

#ifdef INSTANCING
uniform mat4 uVPMatrix;
#else
uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
#endif
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;

#ifndef INSTANCING
uniform vec4 uUVScale;
#endif

#if defined(PLANAR_REFLECTION)
uniform mat4 uRTTMatrix;
#endif

#ifdef SHADOW
uniform mat4 uShadowMatrix;
#endif

out vec2 vTexCoord0;
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
out vec3 vWorldLightDir;
#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
out vec3 vWorldTangent;
out vec3 vWorldBinormal;
out float vTangentW;
#endif

#ifdef SHADOW
out vec3 vDepth;
out vec4 vShadowCoord;
#endif

#if defined(PLANAR_REFLECTION)
out vec4 vReflectCoord;
#endif

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	
#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	vTangentW = inData.x;
#endif

	vec4 worldPos = uWorldMatrix * inPosition;
	vec4 worldViewDir = normalize(uCameraPosition - worldPos);

	vec4 worldNormal = uWorldMatrix * vec4(inNormal.xyz, 0.0);

#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	vec4 worldTangent = uWorldMatrix * vec4(inTangent.xyz, 0.0);
#endif

	vWorldNormal = normalize(worldNormal.xyz);

#if defined(INSTANCING) || (!defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE))
	vWorldTangent = normalize(worldTangent.xyz);
	vWorldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));
#endif

#ifdef SHADOW
	vDepth = uCameraPosition.xyz - worldPos.xyz;
	vShadowCoord = uShadowMatrix * vec4(worldPos.xyz, 1.0);
#endif

#if defined(PLANAR_REFLECTION)
	vReflectCoord = uRTTMatrix * vec4(worldPos.xyz, 1.0);
#endif

	vWorldViewDir = worldViewDir.xyz;
	vWorldLightDir = normalize(uLightDirection.xyz);

#ifdef INSTANCING
	gl_Position = uVPMatrix * worldPos;
#else
	gl_Position = uMvpMatrix * inPosition;
#endif
}