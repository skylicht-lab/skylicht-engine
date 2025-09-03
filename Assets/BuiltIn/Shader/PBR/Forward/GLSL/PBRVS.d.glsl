in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
#if defined(LM)
in vec3 inLightmap;
#else
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inData;
#endif
#endif

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uUVScale;

#ifdef SHADOW
uniform mat4 uShadowMatrix;
#endif

#if defined(PLANAR_REFLECTION)
uniform mat4 uRTTMatrix;
#endif

out vec2 vTexCoord0;
#if defined(LM)
out vec3 vTexCoord1;
#endif
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
out vec3 vWorldTangent;
out vec3 vWorldBinormal;
out float vTangentW;
#endif
out vec4 vViewPosition;
out vec3 vWorldPosition;

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
#if defined(LM)
	vTexCoord1 = inLightmap;
#endif
	
#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	vTangentW = inData.x;
#endif

	vec4 worldPos = uWorldMatrix * inPosition;
	vec4 worldViewDir = normalize(uCameraPosition - worldPos);

	vec4 worldNormal = uWorldMatrix * vec4(inNormal.xyz, 0.0);

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
	vec4 worldTangent = uWorldMatrix * vec4(inTangent.xyz, 0.0);
#endif

	vWorldPosition = worldPos.xyz;
	vWorldNormal = normalize(worldNormal.xyz);

#if !defined(NO_NORMAL_MAP) && !defined(NO_TEXTURE)
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
	vViewPosition = uMvpMatrix * inPosition;

	gl_Position = vViewPosition;
}