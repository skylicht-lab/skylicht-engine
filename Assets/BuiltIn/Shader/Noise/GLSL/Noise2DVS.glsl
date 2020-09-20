in vec4 inPosition;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;

out vec4 varColor;
out vec2 varTexCoord0;

void main(void)
{
	varColor = inColor/255.0;
	varTexCoord0 = 	inTexCoord0;
	gl_Position = uMvpMatrix * inPosition;
}