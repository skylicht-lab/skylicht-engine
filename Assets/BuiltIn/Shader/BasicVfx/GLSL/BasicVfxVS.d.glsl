in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

#ifdef UV2
in vec2 inTexCoord1;
#endif

uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;
#ifdef RIM_LIGHT
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
#endif

out vec2 varTexCoord0;

#ifdef UV2
out vec2 varTexCoord1;
#endif

out vec4 varUVScale;

#ifdef RIM_LIGHT
out vec3 varWorldNormal;
out vec3 varWorldViewDir;
#endif

out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0;
	varUVScale = uUVScale;

#ifdef UV2	
	varTexCoord1 = inTexCoord1;
#endif

	varColor = inColor / 255.0;

#ifdef RIM_LIGHT
	vec4 worldPos = uWorldMatrix * inPosition;
	varWorldViewDir = normalize((uCameraPosition - worldPos).xyz);
	
	vec4 worldNormal = uWorldMatrix * vec4(inNormal, 0.0);
	varWorldNormal = normalize(worldNormal.xyz);
#endif
	gl_Position = uMvpMatrix * inPosition;
}