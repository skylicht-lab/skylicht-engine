layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;
layout(location = 6) in vec2 inData;

layout(location = 7) in vec4 uUVScale;
layout(location = 8) in vec4 uColor;
layout(location = 9) in vec2 uSpecGloss;

layout(location = 10) in mat4 uWorldMatrix;

uniform mat4 uVPMatrix;

out vec4 varPos;
out vec4 varWorldPos;

void main(void)
{
	mat4 uMvpMatrix = uVPMatrix * uWorldMatrix;
	
	gl_Position = uMvpMatrix * inPosition;
	varPos = gl_Position;
	varWorldPos = uWorldMatrix * inPosition;
}