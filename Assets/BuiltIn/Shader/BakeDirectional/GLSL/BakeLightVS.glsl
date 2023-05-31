in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec3 inLightmap;

uniform mat4 uWorldMatrix;

out vec3 varWorldPosition;
out vec3 varWorldNormal;

void main(void)
{
	vec2 lightmapUV = inLightmap.xy;
	lightmapUV.y = 1.0 - lightmapUV.y;
	
	// convert [0 - 1] to [-1 1]
	lightmapUV = lightmapUV * 2.0 - vec2(1.0, 1.0);
	
	varWorldNormal = (uWorldMatrix * vec4(inNormal, 0.0)).xyz;
	varWorldNormal = normalize(varWorldNormal);
	
	varWorldPosition = (uWorldMatrix * inPosition).xyz;
	
	gl_Position = vec4(lightmapUV, 0.0, 1.0);
}