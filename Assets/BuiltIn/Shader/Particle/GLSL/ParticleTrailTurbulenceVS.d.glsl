in vec4 inPosition;
in vec4 inColor;
in vec3 inNormal;
in vec2 inTexCoord0;

uniform mat4 uMvpMatrix;
uniform mat4 uWorld;
uniform vec3 uNoiseParam;

out vec2 varTexCoord0;
out vec4 varColor;

#include "../../Noise/GLSL/LibNoise.glsl"

void main(void)
{
	varTexCoord0 = inTexCoord0;
	varColor = inColor / 255.0;

	vec3 worldPos = (uWorld * inPosition).xyz;

	float n = pnoise(worldPos * uNoiseParam.x);
	float weight = clamp(inTexCoord0.y * uNoiseParam.z, 0.0, 1.0);

	vec4 noisePosition = inPosition + vec4(n * inNormal * weight * uNoiseParam.y, 0.0);

	gl_Position = uMvpMatrix * noisePosition;
}