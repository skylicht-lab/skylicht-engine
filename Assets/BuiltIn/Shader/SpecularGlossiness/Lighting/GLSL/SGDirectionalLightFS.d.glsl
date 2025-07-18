precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform sampler2D uTexLight;
uniform sampler2D uTexIndirect;
uniform sampler2DArray uShadowMap;
#if defined(ENABLE_SSR)
uniform sampler2D uTexLastFrame;
#endif

uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec3 uLightMultiplier;
uniform vec3 uShadowDistance;
uniform mat4 uShadowMatrix[3];

#if defined(ENABLE_SSR)
uniform mat4 uView;
uniform mat4 uProjection;
#endif

in vec2 varTexCoord0;

out vec4 FragColor;

#include "../../../Shadow/GLSL/LibShadow.glsl"
#include "LibSG.glsl"

void main(void)
{
	vec4 albedo = texture(uTexAlbedo, varTexCoord0.xy);
	
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

#if defined(LIGHTMAP_BAKE)
	// backface when render lightmap
	if (dot(viewDir, normal) < 0.0)
	{
		normal = normal * -1.0;
		directMul = 0.0;
		indirectMul = 0.0;
		lightMul = 0.0;
	}
#endif

	// shadow
	float depth = length(v);

	vec4 shadowCoord[3];
	shadowCoord[0] = uShadowMatrix[0] * vec4(position, 1.0);
	shadowCoord[1] = uShadowMatrix[1] * vec4(position, 1.0);
	shadowCoord[2] = uShadowMatrix[2] * vec4(position, 1.0);

	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;

	float visibility = shadow(shadowCoord, shadowDistance, depth);

	// lighting	
	vec3 color = SG(
		albedo.rgb,
		data.r,
		data.g,
		posdepth,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb * uLightColor.a,
		visibility,
		light,
		indirect,
		directMul,
		indirectMul,
		lightMul);

	FragColor = vec4(color, albedo.a);
}