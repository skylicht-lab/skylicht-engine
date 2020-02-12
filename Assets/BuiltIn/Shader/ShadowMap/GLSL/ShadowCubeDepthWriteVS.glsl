in vec4 inPosition;
in vec4 inColor;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
	
out vec4 varWorldPos;

void main(void)
{
	gl_Position = uMvpMatrix * inPosition;
	varWorldPos = uWorldMatrix * inPosition;
}