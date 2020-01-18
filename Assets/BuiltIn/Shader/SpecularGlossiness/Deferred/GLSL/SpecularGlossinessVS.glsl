in vec4 inPosition;
in vec4 inColor;
in vec3 inNormal;
in vec2 inTexCoord0;
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inTangentW;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform vec4 uUVScale;

out vec3 vWorldNormal;
out vec3 vWorldPosition;
out vec3 vWorldTangent;
out vec3 vWorldBinormal;

out vec2 vTexCoord0;
out float vTangentW;

void main(void)
{
	vWorldPosition = (uWorldMatrix*inPosition).xyz;
	
	vec4 worldNormal = uWorldMatrix * vec4(inNormal,0.0);
	vec4 worldTangent = uWorldMatrix * vec4(inTangent,0.0);
		
	vWorldNormal = normalize(worldNormal.xyz);
	vWorldTangent = normalize(worldTangent.xyz);
	vWorldBinormal = cross(vWorldNormal.xyz, vWorldTangent.xyz);
	
	vTexCoord0 = inTexCoord0;// * uUVScale.xy + uUVScale.zw;
	vTangentW = inTangentW.x;
	
	gl_Position = uMvpMatrix * inPosition;
}