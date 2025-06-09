in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
#if defined(LM)
in vec3 inLightmap;
#endif
#if defined(NORMAL_MAP)
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inData;
#endif

uniform mat4 uWorldMatrix;

out vec2 varTex0;
out vec3 varWorldPosition;
out vec3 varWorldNormal;

#if defined(NORMAL_MAP)
out vec3 vWorldTangent;
out vec3 vWorldBinormal;
out float vTangentW;
#endif

void main(void)
{
#if defined(LM)
	vec2 lightmapUV = inLightmap.xy;
#else
	vec2 lightmapUV = inTexCoord0;
#endif	
	
	varTex0 = inTexCoord0;
	
	// convert [0 - 1] to [-1 - 1]
	lightmapUV.y = 1.0 - lightmapUV.y;
	lightmapUV = lightmapUV * 2.0 - vec2(1.0, 1.0);
	
	varWorldNormal = (uWorldMatrix * vec4(inNormal, 0.0)).xyz;
	varWorldNormal = normalize(varWorldNormal);
	
	varWorldPosition = (uWorldMatrix * inPosition).xyz;
	
#if defined(NORMAL_MAP)
	vTangentW = inData.x;
	vec4 worldTangent = uWorldMatrix * vec4(inTangent.xyz, 0.0);
	vWorldTangent = normalize(worldTangent.xyz);
	vWorldBinormal = normalize(cross(varWorldNormal, worldTangent.xyz));
#endif	
	
	gl_Position = vec4(lightmapUV, 0.0, 1.0);
}