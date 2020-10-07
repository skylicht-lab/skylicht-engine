in vec4 inPosition;
in vec4 inColor;
in vec3 inNormal;
in vec2 inTexCoord0;
in vec4 inBlendIndex;
in vec4 inBlendWeight;

uniform mat4 uMvpMatrix;
uniform mat4 uBoneMatrix[64];

out vec2 varTexCoord0;
out vec4 varColor;

void main(void)
{
	mat4 skinMatrix = mat4(0.0);
	vec4 skinPosition = vec4(0.0);
	//vec3 skinNormal = vec3(0.0);

	int index = int(inBlendIndex[0]);
	skinMatrix = inBlendWeight[0] * uBoneMatrix[index];

	index = int(inBlendIndex[1]);
	skinMatrix += inBlendWeight[1] * uBoneMatrix[index];

	index = int(inBlendIndex[2]);
	skinMatrix += inBlendWeight[2] * uBoneMatrix[index];

	index = int(inBlendIndex[3]);
	skinMatrix += inBlendWeight[3] * uBoneMatrix[index];

	skinPosition = skinMatrix * inPosition;
	//skinNormal = (skinMatrix * vec4(inNormal, 0.0)).xyz;

	varTexCoord0 = inTexCoord0;
	varColor = inColor / 255.0;

	gl_Position = uMvpMatrix * skinPosition;
}