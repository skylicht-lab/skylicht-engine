in vec4 inPosition;
in vec4 inColor;
in vec3 inNormal;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;

out vec2 varTexCoord0;

void main(void)
{
	varTexCoord0 = inTexCoord0;
	gl_Position = uMvpMatrix * inPosition;
}