in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;
uniform vec4 uTime;
uniform vec2 uData; // Speed

out vec2 vTexCoord0;
out vec2 vTexCoord1;
out vec4 vColor;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	vTexCoord1 = vec2(inNormal.x, inNormal.y);
	
	float angle = uTime.x * uData.x / 6.0;
	float pi2 = 3.14159265359f * 2.0f;
	
	float cosA = cos(angle * pi2);
	float sinA = sin(angle * pi2);
	
	mat2 rotationMatrix = mat2(
		cosA, -sinA,
		sinA, cosA
	);
	
	vec2 center = vec2(0.5, 0.5);
	vec2 uv = vTexCoord1 - center;
	
	vec2 rotatedUV = rotationMatrix * uv;
	rotatedUV *= 0.7;
	
	rotatedUV += center;
	
	vTexCoord1 = rotatedUV;
	
	vColor = inColor / 255.0;
	gl_Position = uMvpMatrix * inPosition;
}