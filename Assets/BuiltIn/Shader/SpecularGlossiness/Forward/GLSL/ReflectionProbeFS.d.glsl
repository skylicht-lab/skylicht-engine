precision highp float;

uniform samplerCube uTexReflect;

in vec4 vColor;
in vec3 vWorldNormal;

out vec4 FragColor;

#include "../../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	FragColor = vec4(sRGB(textureLod(uTexReflect, vWorldNormal, 0.0).rgb), 1.0);
}