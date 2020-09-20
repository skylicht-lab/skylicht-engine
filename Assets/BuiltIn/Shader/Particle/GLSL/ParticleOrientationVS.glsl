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
layout (location = 10) in vec3 inParticleVelocity;

uniform mat4 uMvpMatrix;
uniform vec3 uOrientationUp;
uniform vec3 uOrientationNormal;

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
	
	float upX = (uOrientationNormal.x * uOrientationNormal.x + (1.0f - uOrientationNormal.x * uOrientationNormal.x) * cosA) * uOrientationUp.x
		+ (uOrientationNormal.x * uOrientationNormal.y * oneMinusCosA - uOrientationNormal.z * sinA) * uOrientationUp.y
		+ (uOrientationNormal.x * uOrientationNormal.z * oneMinusCosA + uOrientationNormal.y * sinA) * uOrientationUp.z;
	
	float upY = (uOrientationNormal.x * uOrientationNormal.y * oneMinusCosA + uOrientationNormal.z * sinA) * uOrientationUp.x
		+ (uOrientationNormal.y * uOrientationNormal.y + (1.0f - uOrientationNormal.y * uOrientationNormal.y) * cosA) * uOrientationUp.y
		+ (uOrientationNormal.y * uOrientationNormal.z * oneMinusCosA - uOrientationNormal.x * sinA) * uOrientationUp.z;

	float upZ = (uOrientationNormal.x * uOrientationNormal.z * oneMinusCosA - uOrientationNormal.y * sinA) * uOrientationUp.x
		+ (uOrientationNormal.y * uOrientationNormal.z * oneMinusCosA + uOrientationNormal.x * sinA) * uOrientationUp.y
		+ (uOrientationNormal.z * uOrientationNormal.z + (1.0f - uOrientationNormal.z * uOrientationNormal.z) * cosA) * uOrientationUp.z;
	
	// billboard position
	vec3 up = vec3(upX, upY, upZ);
	up = normalize(up);
	
	vec3 side = cross(up, uOrientationNormal);
	side = normalize(side);	
	
	side = side * 0.5 * inParticleSize.x;	
	up = up * 0.5 * inParticleSize.y;
	
	vec3 position = inParticlePosition + inPosition.x * side + inPosition.y * up;
	
	gl_Position = uMvpMatrix * vec4(position, 1.0);
}