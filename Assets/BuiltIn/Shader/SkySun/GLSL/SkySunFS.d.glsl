precision mediump float;

uniform vec4 uColor;
uniform vec2 uIntensity;

uniform vec4 uLightDirection;
uniform vec4 uCamPosition;

in vec2 varTexCoord0;
in vec4 varColor;
in vec4 varWorldPos;

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec3 viewDir = normalize(varWorldPos.xyz - uCamPosition.xyz);
	
	float y = 1.0 - (max(viewDir.y, 0.0) * 0.8 + 0.2) * 0.8;

	vec3 skyColor = vec3(pow(y, 2.0), y, 0.6 + y*0.4) * 1.1;
	vec3 groundColor = vec3(0.4, 0.4, 0.4);
	
	vec3 result = mix(skyColor, sRGB(groundColor), pow(smoothstep(0.0,-0.025, viewDir.y), 0.2));
	vec4 blend = varColor * uColor;
	
	FragColor = vec4(result * sRGB(blend.rgb), blend.a);
}