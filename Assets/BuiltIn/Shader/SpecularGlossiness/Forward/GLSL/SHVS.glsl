in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;

out vec4 vColor;
out vec3 vWorldNormal;

void main(void)
{
	vColor = inColor / 255.0;

	vec4 worldNormal = uWorldMatrix * vec4(inNormal, 0.0);
	vWorldNormal = normalize(worldNormal.xyz);

	gl_Position = uMvpMatrix * inPosition;
}