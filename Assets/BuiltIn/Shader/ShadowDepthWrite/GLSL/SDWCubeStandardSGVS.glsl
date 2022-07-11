layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;

layout(location = 4) in vec4 uUVScale;
layout(location = 5) in vec4 uColor;
layout(location = 6) in vec2 uSpecGloss;

layout(location = 7) in vec3 sh0;
layout(location = 8) in vec3 sh1;
layout(location = 9) in vec3 sh2;
layout(location = 10) in vec3 sh3;

layout(location = 11) in mat4 uWorldMatrix;

uniform mat4 uVPMatrix;

out vec4 varPos;

void main(void)
{
	mat4 uMvpMatrix = uVPMatrix * uWorldMatrix;
	
	gl_Position = uMvpMatrix * inPosition;
	varPos = uWorldMatrix * inPosition;
}