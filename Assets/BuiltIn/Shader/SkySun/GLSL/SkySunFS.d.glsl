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
#include "LibSkySun.glsl"

void main(void)
{
	vec3 viewDir = normalize(varWorldPos.xyz - uCamPosition.xyz);
	
	// SKY & SUN
	vec3 skyColor = GetSkyColor(
		viewDir,
		uLightDirection.xyz,
		1.1,						// intensiy
		vec4(1.0, 0.8, 0.7, 0.1),	// atmospheric, intensiy
		vec4(1.0, 0.6, 0.1, 0.5), 	// sun, intensiy
		vec4(1.0, 0.6, 0.1, 0.4),	// glare1, intensiy
		vec4(1.0, 0.4, 0.2, 0.2),	// glare2, intensiy
		800.0						// sun radius
	);
	
	// GROUND
	vec3 groundColor = vec3(0.4, 0.4, 0.4);
	
	// RESULT
	vec3 result = mix(skyColor, sRGB(groundColor), pow(smoothstep(0.0,-0.025, viewDir.y), 0.2));
	vec4 blend = varColor * uColor;
	
	FragColor = vec4(result * sRGB(blend.rgb), blend.a);
}