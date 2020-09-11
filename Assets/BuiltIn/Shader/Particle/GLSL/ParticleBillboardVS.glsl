layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord0;

layout (location = 4) in vec3 inParticlePosition;
layout (location = 5) in vec4 inParticleColor;
layout (location = 6) in vec2 inParticleUVScale;
layout (location = 7) in vec2 inParticleUVOffset;
layout (location = 8) in vec3 inParticleSize;
layout (location = 9) in vec3 inParticleRotation;

uniform mat4 uMvpMatrix;
uniform vec3 uViewUp;
uniform vec3 uViewLook;

out vec2 varTexCoord0;
out vec4 varColor;

void main(void)
{
	varTexCoord0 = inTexCoord0 * inParticleUVScale + inParticleUVOffset;
	varColor = inParticleColor/255.0;
	
	// rotate
	float cosA = cos(inParticleRotation.z);
	float sinA = sin(inParticleRotation.z);
	float oneMinusCosA = 1.0 - cosA;
	
	float upX = (uViewLook.x * uViewLook.x + (1.0f - uViewLook.x * uViewLook.x) * cosA) * uViewUp.x
		+ (uViewLook.x * uViewLook.y * oneMinusCosA - uViewLook.z * sinA) * uViewUp.y
		+ (uViewLook.x * uViewLook.z * oneMinusCosA + uViewLook.y * sinA) * uViewUp.z;
	
	float upY = (uViewLook.x * uViewLook.y * oneMinusCosA + uViewLook.z * sinA) * uViewUp.x
		+ (uViewLook.y * uViewLook.y + (1.0f - uViewLook.y * uViewLook.y) * cosA) * uViewUp.y
		+ (uViewLook.y * uViewLook.z * oneMinusCosA - uViewLook.x * sinA) * uViewUp.z;

	float upZ = (uViewLook.x * uViewLook.z * oneMinusCosA - uViewLook.y * sinA) * uViewUp.x
		+ (uViewLook.y * uViewLook.z * oneMinusCosA + uViewLook.x * sinA) * uViewUp.y
		+ (uViewLook.z * uViewLook.z + (1.0f - uViewLook.z * uViewLook.z) * cosA) * uViewUp.z;
	
	// billboard position
	vec3 up = vec3(upX, upY, upZ);
	up = normalize(up);
	
	vec3 side = cross(up, uViewLook);
	side = normalize(side);	
	
	side = side * 0.5 * inParticleSize.x;	
	up = up * 0.5 * inParticleSize.y;
	
	vec3 position = inParticlePosition + inPosition.x * side + inPosition.y * up;
	
	gl_Position = uMvpMatrix * vec4(position, 1.0);
}