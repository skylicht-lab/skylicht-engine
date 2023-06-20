precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform sampler2D uTexLight;
uniform sampler2D uTexIndirect;

#if defined(ENABLE_SSR)
uniform sampler2D uTexLastFrame;
#endif

uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec3 uLightMultiplier;

#if defined(ENABLE_SSR)
uniform mat4 uView;
uniform mat4 uProjection;
#endif

in vec2 varTexCoord0;

out vec4 FragColor;

#include "LibSGLM.glsl"

void main(void)
{
	vec3 albedo = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	
	vec4 posdepth = texture(uTexPosition, varTexCoord0.xy);
	vec3 position = posdepth.xyz;
	
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
	vec4 light = texture(uTexLight, varTexCoord0.xy);
	vec3 indirect = texture(uTexIndirect, varTexCoord0.xy).rgb;

	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);

	float directMul = uLightMultiplier.x;
	float indirectMul = uLightMultiplier.y;
	float lightMul = uLightMultiplier.z;

	// lighting	
	vec3 color = SGLM(
		albedo,
		data.r,
		data.g,
		posdepth,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb * uLightColor.a,
		light,
		indirect,
		directMul,
		indirectMul,
		lightMul);

	FragColor = vec4(color, 1.0);
}