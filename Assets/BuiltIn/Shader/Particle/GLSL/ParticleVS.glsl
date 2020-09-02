layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord0;

layout (location = 4) in vec3 inParticlePosition;
layout (location = 5) in vec4 inParticleColor;
layout (location = 6) in vec2 inParticleUVScale;
layout (location = 7) in vec2 inParticleUVOffset;
layout (location = 8) in vec2 inParticleSizeRotation;

uniform mat4 uMvpMatrix;

out vec2 varTexCoord0;
out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0 * inParticleUVScale + inParticleUVOffset;
	varColor = inParticleColor/255.0;
	
	vec3 position = inPosition.xyz * inParticleSizeRotation.x + inParticlePosition;
	
	gl_Position = uMvpMatrix * vec4(position, 1.0);
}