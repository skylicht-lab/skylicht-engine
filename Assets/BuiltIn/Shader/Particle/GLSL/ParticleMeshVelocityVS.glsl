layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord0;

layout(location = 4) in vec4 inParticlePosition;
layout(location = 5) in vec4 inParticleColor;
layout(location = 6) in vec2 inParticleUVScale;
layout(location = 7) in vec2 inParticleUVOffset;
layout(location = 8) in vec4 inParticleSize;
layout(location = 9) in vec4 inParticleRotation;
layout(location = 10) in vec4 inParticleVelocity;

uniform mat4 uVPMatrix;
uniform mat4 uWorld;

out vec2 varTexCoord0;
out vec4 varColor;
out vec3 varWorldNormal;
out vec3 varVertexPos;

mat4 constructMat4(vec3 position, vec3 zAxis, vec3 scale, vec3 rotation) 
{
	// rotate
	float cosYaw = cos(rotation.z);
	float sinYaw = sin(rotation.z);	
	float cosPitch = cos(rotation.y);
	float sinPitch = sin(rotation.y);
	float cosRoll = cos(rotation.x);
	float sinRoll = sin(rotation.x);

	mat4 rotationMatrix = mat4(
		cosYaw * cosPitch, sinYaw * cosPitch, -sinPitch, 0.0,
		cosYaw * sinPitch * sinRoll - sinYaw * cosRoll, sinYaw * sinPitch * sinRoll + cosYaw * cosRoll, cosPitch * sinRoll, 0.0,
		cosYaw * sinPitch * cosRoll + sinYaw * sinRoll, sinYaw * sinPitch * cosRoll - cosYaw * sinRoll, cosPitch * cosRoll, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	
	/*
	vec3 yTemp = vec3(0.0, 1.0, 0.0);
	
	// vel (z) is y up
	if (abs(dot(zAxis, yTemp)) > 0.999) 
		yTemp = vec3(0.0, 0.0, 1.0);
	*/
	
	float dot_abs = abs(dot(zAxis, vec3(0.0, 1.0, 0.0)));
	float mask = clamp(ceil(dot_abs - 0.999), 0.0, 1.0);
	vec3 yTemp = vec3(0.0, 1.0 - mask, mask);
	
	vec3 xAxis = normalize(cross(yTemp, zAxis));
	vec3 yAxis = normalize(cross(zAxis, xAxis));
	
	xAxis *= scale.x;
	yAxis *= scale.y;
	zAxis *= scale.z;

	mat4 m = mat4(
		xAxis.x, xAxis.y, xAxis.z, 0.0,
		yAxis.x, yAxis.y, yAxis.z, 0.0,
		zAxis.x, zAxis.y, zAxis.z, 0.0,
		position.x, position.y, position.z, 1.0
	);
	
	return m * rotationMatrix;
}

void main(void)
{
	varTexCoord0 = inTexCoord0 * inParticleUVScale + inParticleUVOffset;
	varColor = inParticleColor / 255.0;	

	vec3 vel = normalize(inParticleVelocity.xyz);

	mat4 world = uWorld * constructMat4(inParticlePosition.xyz, vel, inParticleSize.xyz, inParticleRotation.xyz);

	vec4 worldPos = world * inPosition;
	vec4 worldNormal = world * vec4(inNormal, 1.0);
	
	varWorldNormal = normalize(worldNormal.xyz);
	varVertexPos = inPosition.xyz;
	gl_Position = uVPMatrix * worldPos;
}