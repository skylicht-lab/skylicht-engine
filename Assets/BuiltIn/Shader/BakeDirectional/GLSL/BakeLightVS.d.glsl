in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
#if defined(LM)
in vec3 inLightmap;
#endif

uniform mat4 uWorldMatrix;

out vec3 varWorldPosition;
out vec3 varWorldNormal;

void main(void)
{
#if defined(LM)
	vec2 lightmapUV = inLightmap.xy;
#else
	vec2 lightmapUV = inTexCoord0;
#endif	
	
	// convert [0 - 1] to [-1 - 1]
	lightmapUV.y = 1.0 - lightmapUV.y;
	lightmapUV = lightmapUV * 2.0 - vec2(1.0, 1.0);
	
	varWorldNormal = (uWorldMatrix * vec4(inNormal, 0.0)).xyz;
	varWorldNormal = normalize(varWorldNormal);
	
	varWorldPosition = (uWorldMatrix * inPosition).xyz;
	
	gl_Position = vec4(lightmapUV, 0.0, 1.0);
}