in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;

out vec4 vColor;

void main(void)
{
	vColor = inColor / 255.0;	
	gl_Position = uMvpMatrix * inPosition;
}