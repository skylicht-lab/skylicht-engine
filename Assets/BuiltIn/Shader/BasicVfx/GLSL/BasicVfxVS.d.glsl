in vec4 inPosition;
in vec4 inColor;
in vec2 inTexCoord0;

#ifdef UV2
in vec2 inTexCoord1;
#endif

uniform mat4 uMvpMatrix;

out vec2 varTexCoord0;

#ifdef UV2
out vec2 varTexCoord1;
#endif

out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0;

#ifdef UV2	
	varTexCoord1 = inTexCoord1;
#endif

	varColor = inColor / 255.0;

	gl_Position = uMvpMatrix * inPosition;
}