layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;
layout(location = 6) in vec2 inData;
layout(location = 7) in vec4 inBlendIndex;
layout(location = 8) in vec4 inBlendWeight;

layout(location = 9) in vec4 uBoneLocation;
layout(location = 10) in vec4 uColor;
layout(location = 11) in vec2 uBlendAnimation;

layout(location = 12) in mat4 uWorldMatrix;

uniform sampler2D uTransformTexture;

uniform mat4 uVPMatrix;
uniform vec2 uBoneCount;
uniform vec2 uTransformTextureSize;

out vec4 varPos;
out vec4 varWorldPos;

#include "../../TransformTexture/GLSL/LibTransformTexture.glsl"

void main(void)
{
	mat4 skinMatrix = mat4(0.0);
	vec4 skinPosition = vec4(0.0);
	
	vec2 boneLocation = uBoneLocation.xy;
	float boneLocationY = uBoneLocation.y * uBoneCount.x;

	boneLocation.y = boneLocationY + inBlendIndex[0];
	skinMatrix = inBlendWeight[0] * getTransformFromTexture(boneLocation);

	boneLocation.y = boneLocationY + inBlendIndex[1];
	skinMatrix += inBlendWeight[1] * getTransformFromTexture(boneLocation);

	boneLocation.y = boneLocationY + inBlendIndex[2];
	skinMatrix += inBlendWeight[2] * getTransformFromTexture(boneLocation);

	boneLocation.y = boneLocationY + inBlendIndex[3];
	skinMatrix += inBlendWeight[3] * getTransformFromTexture(boneLocation);

	skinPosition = skinMatrix * inPosition;
	
	varWorldPos = uWorldMatrix * skinPosition;
	gl_Position = uVPMatrix * varWorldPos;
	varPos = gl_Position;
}