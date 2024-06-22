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

uniform mat4 uVPMatrix;
uniform vec4 uCameraPosition;

out vec2 vTexCoord0;
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
out vec3 vWorldPosition;
out vec3 vDepth;
out vec4 vColor;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	
	vec4 worldPos = uWorldMatrix * inPosition;
	vec4 worldNormal = uWorldMatrix * vec4(inNormal.xyz, 0.0);
	
	vWorldPosition = worldPos.xyz;
	vDepth = uCameraPosition.xyz - vWorldPosition;
	
	vWorldNormal = normalize(worldNormal.xyz);
	vWorldViewDir = normalize(vDepth);
	vColor = uColor * inColor / 255.0;
	
	gl_Position = uVPMatrix * worldPos;
}