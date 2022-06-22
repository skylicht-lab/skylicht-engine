in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec3 inLightmap;

uniform mat4 uMvpMatrix;

out vec3 varTexCoord0;

void main(void)
{
	varTexCoord0 = inLightmap;
	gl_Position = uMvpMatrix * inPosition;
}