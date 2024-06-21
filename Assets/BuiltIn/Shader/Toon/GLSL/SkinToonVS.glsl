in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
in vec4 inBlendIndex;
in vec4 inBlendWeight;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
uniform vec4 uUVScale;
uniform mat4 uBoneMatrix[64];

out vec2 vTexCoord0;
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
out vec3 vWorldPosition;
out vec3 vDepth;
out vec4 vColor;

void main(void)
{
	mat4 skinMatrix = mat4(0.0);
	vec4 skinPosition = vec4(0.0);
	vec3 skinNormal = vec3(0.0);

	int index = int(inBlendIndex[0]);
	skinMatrix = inBlendWeight[0] * uBoneMatrix[index];

	index = int(inBlendIndex[1]);
	skinMatrix += inBlendWeight[1] * uBoneMatrix[index];

	index = int(inBlendIndex[2]);
	skinMatrix += inBlendWeight[2] * uBoneMatrix[index];

	index = int(inBlendIndex[3]);
	skinMatrix += inBlendWeight[3] * uBoneMatrix[index];

	skinPosition = skinMatrix * inPosition;
	skinNormal = (skinMatrix * vec4(inNormal, 0.0)).xyz;
	
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	
	vec4 worldPos = uWorldMatrix * skinPosition;
	vec4 worldNormal = uWorldMatrix * vec4(skinNormal, 0.0);
	
	vWorldPosition = worldPos.xyz;
	vDepth = uCameraPosition.xyz - vWorldPosition;
	
	vWorldNormal = normalize(worldNormal.xyz);
	vWorldViewDir = normalize(vDepth);
	
	vColor = inColor / 255.0;
	
	gl_Position = uMvpMatrix * skinPosition;
}