in vec4 inPosition;
in vec4 inColor;

uniform mat4 uMvpMatrix;
uniform mat4 uWorld;

out vec4 varColor;
out vec4 varWorldPos;

void main(void)
{
	varColor = inColor / 255.0;
	varWorldPos = inPosition;
	gl_Position = uMvpMatrix * inPosition;
}