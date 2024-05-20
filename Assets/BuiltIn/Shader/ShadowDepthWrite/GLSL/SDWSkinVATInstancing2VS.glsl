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

uniform sampler2D uVertexPositionTexture;

uniform mat4 uVPMatrix;
uniform vec2 uTextureSize;

out vec4 varPos;
out vec4 varWorldPos;

void main(void)
{
	mat4 skinMatrix = mat4(0.0);
	
	vec2 uv;
	
	// Vertex id
	uv.x = (inData.y + 0.5) / uTextureSize.x;
	
	// ANIMATION 1
	uv.y = (uBoneLocation.x + uBoneLocation.y + 0.5) / uTextureSize.y;
	vec4 skinPosition1 = textureLod(uVertexPositionTexture, uv, 0.0);
	
	// ANIMATION 2
	uv.y = (uBoneLocation.z + uBoneLocation.w + 0.5) / uTextureSize.y;
	vec4 skinPosition2 = textureLod(uVertexPositionTexture, uv, 0.0);
	
	// blend animations
	vec4 skinPosition = mix(skinPosition1, skinPosition2, uBlendAnimation.x);
	
	varWorldPos = uWorldMatrix * vec4(skinPosition.xyz, 1.0);
	gl_Position = uVPMatrix * varWorldPos;
	varPos = gl_Position;
}