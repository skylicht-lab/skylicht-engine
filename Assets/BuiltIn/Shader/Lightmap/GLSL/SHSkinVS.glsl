in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
in vec4 inBlendIndex;
in vec4 inBlendWeight;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform mat4 uBoneMatrix[64];

out vec3 vWorldNormal;

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
	
	vec4 worldNormal = uWorldMatrix * vec4(skinNormal, 0.0);
	vWorldNormal = normalize(worldNormal.xyz);

	gl_Position = uMvpMatrix * skinPosition;
}