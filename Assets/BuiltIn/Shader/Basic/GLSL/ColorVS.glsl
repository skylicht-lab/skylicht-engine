in vec4 inPosition;
in vec4 inColor;

uniform mat4 uMvpMatrix;
	
out vec4 varColor;

void main(void)
{
	varColor = inColor/255.0;
	gl_Position = uMvpMatrix * inPosition;
}