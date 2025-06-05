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
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inData;
#endif

#ifdef INSTANCING
uniform mat4 uVPMatrix;
uniform mat4 uView;
#else
uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform mat4 uView;
uniform vec4 uUVScale;
#endif

out vec3 vWorldNormal;
out vec4 vWorldPosition;
out vec3 vWorldTangent;
out vec3 vWorldBinormal;

out vec2 vTexCoord0;
out float vTangentW;

#ifdef INSTANCING
out vec4 vColor;
out vec2 vSpecGloss;
#endif

void main(void)
{
#ifdef INSTANCING
	vColor = uColor;
	vSpecGloss = uSpecGloss.xy;
#endif

	vec4 worldPosition = uWorldMatrix*inPosition;

	vec4 sampleFragPos = uView * worldPosition;
	vWorldPosition = vec4(worldPosition.xyz, sampleFragPos.z);

	vec4 worldNormal = uWorldMatrix * vec4(inNormal, 0.0);
	vec4 worldTangent = uWorldMatrix * vec4(inTangent, 0.0);

	vWorldNormal = normalize(worldNormal.xyz);
	vWorldTangent = normalize(worldTangent.xyz);
	vWorldBinormal = normalize(cross(vWorldNormal.xyz, vWorldTangent.xyz));

	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	vTangentW = inData.x;

#ifdef INSTANCING
	gl_Position = uVPMatrix * worldPosition;
#else
	gl_Position = uMvpMatrix * inPosition;
#endif
}