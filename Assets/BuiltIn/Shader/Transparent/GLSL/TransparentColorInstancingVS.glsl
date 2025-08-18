layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec4 uUVScale;
layout(location = 5) in vec4 uColor;
layout(location = 6) in mat4 uWorldMatrix;

uniform mat4 uVPMatrix;

out vec2 varTexCoord0;
out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;;
	varColor = uColor;
	
	vec4 worldPos = uWorldMatrix * inPosition;
	gl_Position = uVPMatrix * worldPos;
}