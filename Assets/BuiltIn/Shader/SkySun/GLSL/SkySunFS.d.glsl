precision mediump float;

uniform vec4 uLightDirection;
uniform vec4 uCamPosition;

uniform vec4 uIntensity;
uniform vec4 uAtmospheric;
uniform vec4 uSun;
uniform vec4 uGlare1;
uniform vec4 uGlare2;
	
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
		uIntensity.x,				// intensiy
		uAtmospheric,				// atmospheric, intensiy
		uSun, 						// sun, intensiy
		uGlare1,					// glare1, intensiy
		uGlare2,					// glare2, intensiy
		uIntensity.y				// sun radius
	);
	
	// GROUND
	vec3 groundColor = vec3(0.4, 0.4, 0.4);
	
	// RESULT
	vec3 result = mix(skyColor, sRGB(groundColor), pow(smoothstep(0.0,-0.025, viewDir.y), 0.2));	
	FragColor = vec4(result, 1.0);
}