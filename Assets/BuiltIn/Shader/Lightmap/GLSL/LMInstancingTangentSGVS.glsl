layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;
layout(location = 6) in vec2 inData;

layout(location = 7) in vec3 D0;
layout(location = 8) in vec3 D1;
layout(location = 9) in vec3 D2;
layout(location = 10) in vec3 D3;

layout(location = 11) in mat4 uWorldMatrix;

uniform mat4 uVPMatrix;

out vec3 vWorldNormal;

out vec3 vD0;
out vec3 vD1;
out vec3 vD2;
out vec3 vD3;

void main(void)
{
	mat4 uMvpMatrix = uVPMatrix * uWorldMatrix;

	vec4 worldNormal = uWorldMatrix * vec4(inNormal, 0.0);
	vWorldNormal = normalize(worldNormal.xyz);

	vD0 = D0;
	vD1 = D1;
	vD2 = D2;
	vD3 = D3;

	gl_Position = uMvpMatrix * inPosition;
}