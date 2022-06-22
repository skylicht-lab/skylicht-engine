in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;

out vec2 varTexCoord0;
out vec4 varColor;
out vec4 varWorldPos;

void main(void)
{
	varTexCoord0 = inTexCoord0;
	varColor = inColor / 255.0;
	varWorldPos = uWorldMatrix * inPosition;
	gl_Position = uMvpMatrix * inPosition;
}