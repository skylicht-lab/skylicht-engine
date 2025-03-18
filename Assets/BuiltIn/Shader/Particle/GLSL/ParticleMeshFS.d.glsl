precision mediump float;

uniform sampler2D uTexture;

in vec2 varTexCoord0;
in vec4 varColor;
in vec3 varWorldNormal;

out vec4 FragColor;

uniform vec4 uColorIntensity;

#ifdef LIGHTING
uniform vec4 uLightColor;
uniform vec4 uLightDirection;	
uniform vec4 uSHConst[4];
#endif

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

#ifdef LIGHTING
#include "../../SHAmbient/GLSL/SHAmbient.glsl"
#endif

const float PI = 3.1415926;

void main(void)
{
	vec4 texColor = texture(uTexture, varTexCoord0.xy);
	vec3 color = sRGB(texColor.rgb * varColor.rgb * uColorIntensity.rgb);
	
#ifdef LIGHTING
	// Direction lighting
	float NdotL = max(dot(varWorldNormal, uLightDirection.xyz), 0.0);
	vec3 directionalLight = NdotL * sRGB(uLightColor.rgb);
	color = directionalLight * color * 0.3;
	
	// SH4 Ambient
	vec3 ambientLighting = shAmbient(varWorldNormal);
	color += sRGB(ambientLighting) * texColor.rgb / PI;
#endif	
	
	FragColor = vec4(color, texColor.a * varColor.a);
}