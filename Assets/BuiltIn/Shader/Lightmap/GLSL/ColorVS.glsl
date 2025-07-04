in vec4 inPosition;
in vec4 inColor;

uniform mat4 uMvpMatrix;

void main(void)
{
	gl_Position = uMvpMatrix * inPosition;
}