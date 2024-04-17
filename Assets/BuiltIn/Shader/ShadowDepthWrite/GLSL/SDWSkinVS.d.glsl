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
layout(location = 11) in vec4 uSpecGloss;

layout(location = 12) in mat4 uWorldMatrix;

uniform sampler2D uTransformTexture;

uniform mat4 uVPMatrix;
uniform vec4 uAnimation;
uniform vec2 uTransformTextureSize;

out vec4 varPos;

#include "../../TransformTexture/GLSL/LibTransformTexture.glsl"

void main(void)
{
	mat4 uMvpMatrix = uVPMatrix * uWorldMatrix;
	
	mat4 skinMatrix = mat4(0.0);
	vec4 skinPosition = vec4(0.0);
	
	vec2 boneLocation = uBoneLocation.xy;

	boneLocation.y = inBlendIndex[0];
	skinMatrix = inBlendWeight[0] * getTransformFromTexture(boneLocation);

	boneLocation.y = inBlendIndex[1];
	skinMatrix += inBlendWeight[1] * getTransformFromTexture(boneLocation);

	boneLocation.y = inBlendIndex[2];
	skinMatrix += inBlendWeight[2] * getTransformFromTexture(boneLocation);

	boneLocation.y = inBlendIndex[3];
	skinMatrix += inBlendWeight[3] * getTransformFromTexture(boneLocation);

	skinPosition = skinMatrix * inPosition;
	
	gl_Position = uMvpMatrix * skinPosition;
	varPos = gl_Position;
}