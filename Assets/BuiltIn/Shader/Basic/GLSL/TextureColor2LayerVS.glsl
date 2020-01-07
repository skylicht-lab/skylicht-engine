in vec4 inPosition;
in vec4 inColor;
in vec2 inTexCoord0;
in vec2 inTexCoord1;

uniform mat4 uMvpMatrix;

out vec2 varTexCoord0;
out vec2 varTexCoord1;
out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0;
	varTexCoord1 = inTexCoord1;
	varColor = inColor/255.0;
	
	gl_Position = uMvpMatrix * inPosition;
}