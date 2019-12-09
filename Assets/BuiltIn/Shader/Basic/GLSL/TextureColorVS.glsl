in vec4 	inPosition;
in vec4 	inColor;
in vec3 	inNormal;
in vec2 	inTexCoord0;

uniform mat4 		uMvpMatrix;
	
out vec2 varTexCoord0;
out vec4 varColor;

void main(void)
{
	// ---------------------------------
	// RESULT 
	// ---------------------------------
	varTexCoord0 = inTexCoord0;
	varColor = inColor/255.0;
	
	gl_Position = uMvpMatrix * inPosition;
}