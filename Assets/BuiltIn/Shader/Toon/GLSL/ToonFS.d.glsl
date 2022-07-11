precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexRamp;

uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec4 uShadowColor;
uniform vec2 uWrapFactor;
uniform vec3 uSpecular;

in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

void main(void)
{
	vec3 diffuseMap = sRGB(texture(uTexDiffuse, vTexCoord0.xy).rgb);
	
	float NdotL = max((dot(vWorldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x), 0.0);
	
	vec3 rampMap = texture(uTexRamp, vec2(NdotL, NdotL)).rgb;
	
	vec3 color = sRGB(uColor.rgb);
	vec3 shadowColor = sRGB(uShadowColor.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);
	
	// Shadows intensity through alpha
	vec3 ramp = mix(color, shadowColor, uColor.a);
	ramp = mix(ramp, color, rampMap);
	
	// Specular
	vec3 h = normalize(uLightDirection.xyz + vWorldViewDir);
	float NdotH = max(0, dot(vWorldNormal, h));
	
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	
	FragColor = vec4(diffuseMap * lightColor * ramp + lightColor * spec, 1.0);
}